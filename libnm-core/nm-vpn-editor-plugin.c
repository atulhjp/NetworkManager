/* -*- Mode: C; tab-width: 4; indent-tabs-mode: t; c-basic-offset: 4 -*- */
/*
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the
 * Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301 USA.
 *
 * Copyright 2008 Novell, Inc.
 * Copyright 2008 - 2010 Red Hat, Inc.
 * Copyright 2015 Red Hat, Inc.
 */

#include "nm-default.h"

#include "nm-vpn-editor-plugin.h"

#include <dlfcn.h>
#include <gmodule.h>
#include <gobject/gvaluecollector.h>

#include "nm-core-internal.h"

static void nm_vpn_editor_plugin_default_init (NMVpnEditorPluginInterface *iface);

G_DEFINE_INTERFACE (NMVpnEditorPlugin, nm_vpn_editor_plugin, G_TYPE_OBJECT)

static void
nm_vpn_editor_plugin_default_init (NMVpnEditorPluginInterface *iface)
{
	/* Properties */

	/**
	 * NMVpnEditorPlugin:name:
	 *
	 * Short display name of the VPN plugin.
	 */
	g_object_interface_install_property (iface,
		 g_param_spec_string (NM_VPN_EDITOR_PLUGIN_NAME, "", "",
		                      NULL,
		                      G_PARAM_READABLE |
		                      G_PARAM_STATIC_STRINGS));

	/**
	 * NMVpnEditorPlugin:description:
	 *
	 * Longer description of the VPN plugin.
	 */
	g_object_interface_install_property (iface,
		 g_param_spec_string (NM_VPN_EDITOR_PLUGIN_DESCRIPTION, "", "",
		                      NULL,
		                      G_PARAM_READABLE |
		                      G_PARAM_STATIC_STRINGS));

	/**
	 * NMVpnEditorPlugin:service:
	 *
	 * D-Bus service name of the plugin's VPN service.
	 */
	g_object_interface_install_property (iface,
		 g_param_spec_string (NM_VPN_EDITOR_PLUGIN_SERVICE, "", "",
		                      NULL,
		                      G_PARAM_READABLE |
		                      G_PARAM_STATIC_STRINGS));
}

/*********************************************************************/

typedef struct {
	NMVpnPluginInfo *plugin_info;
} NMVpnEditorPluginPrivate;

static void
_private_destroy (gpointer data)
{
	NMVpnEditorPluginPrivate *priv = data;

	if (priv->plugin_info)
		g_object_remove_weak_pointer ((GObject *) priv->plugin_info, (gpointer *) &priv->plugin_info);

	g_slice_free (NMVpnEditorPluginPrivate, priv);
}

static NMVpnEditorPluginPrivate *
_private_get (NMVpnEditorPlugin *plugin, gboolean create)
{
	static GQuark quark = 0;
	NMVpnEditorPluginPrivate *priv;

	nm_assert (NM_IS_VPN_EDITOR_PLUGIN (plugin));

	if (G_UNLIKELY (quark == 0))
		quark = g_quark_from_string ("nm-vpn-editor-plugin-private");

	priv = g_object_get_qdata ((GObject *) plugin, quark);
	if (G_LIKELY (priv))
		return priv;
	if (!create)
		return NULL;
	priv = g_slice_new0 (NMVpnEditorPluginPrivate);
	g_object_set_qdata_full ((GObject *) plugin, quark, priv, _private_destroy);
	return priv;
}

#define NM_VPN_EDITOR_PLUGIN_GET_PRIVATE(plugin)     _private_get (plugin, TRUE)
#define NM_VPN_EDITOR_PLUGIN_TRY_GET_PRIVATE(plugin) _private_get (plugin, FALSE)

/*********************************************************************/

/**
 * nm_vpn_editor_plugin_get_plugin_info:
 * @plugin: the #NMVpnEditorPlugin instance
 *
 * Returns: (transfer-none): if set, return the #NMVpnPluginInfo instance.
 *
 * Since: 1.4
 */
NMVpnPluginInfo *
nm_vpn_editor_plugin_get_plugin_info (NMVpnEditorPlugin *plugin)
{
	NMVpnEditorPluginPrivate *priv;

	g_return_val_if_fail (NM_IS_VPN_EDITOR_PLUGIN (plugin), NULL);

	priv = NM_VPN_EDITOR_PLUGIN_TRY_GET_PRIVATE (plugin);
	return priv ? priv->plugin_info : NULL;
}

/**
 * nm_vpn_editor_plugin_set_plugin_info:
 * @plugin: the #NMVpnEditorPlugin instance
 * @plugin_info: (allow-none): a #NMVpnPluginInfo instance or %NULL
 *
 * Returns: (transfer-none): set or clear the plugin-info instance.
 *   This takes a weak reference on @plugin_info, to avoid circular
 *   reference as the plugin-info might also reference the editor-plugin.
 *
 * Since: 1.4
 */
void
nm_vpn_editor_plugin_set_plugin_info (NMVpnEditorPlugin *plugin, NMVpnPluginInfo *plugin_info)
{
	NMVpnEditorPluginInterface *interface;
	NMVpnEditorPluginPrivate *priv;

	g_return_if_fail (NM_IS_VPN_EDITOR_PLUGIN (plugin));

	if (!plugin_info) {
		priv = NM_VPN_EDITOR_PLUGIN_TRY_GET_PRIVATE (plugin);
		if (!priv)
			return;
	} else {
		g_return_if_fail (NM_IS_VPN_PLUGIN_INFO (plugin_info));
		priv = NM_VPN_EDITOR_PLUGIN_GET_PRIVATE (plugin);
	}

	if (priv->plugin_info == plugin_info)
		return;
	if (priv->plugin_info)
		g_object_remove_weak_pointer ((GObject *) priv->plugin_info, (gpointer *) &priv->plugin_info);
	priv->plugin_info = plugin_info;
	if (priv->plugin_info)
		g_object_add_weak_pointer ((GObject *) priv->plugin_info, (gpointer *) &priv->plugin_info);

	if (plugin_info) {
		interface = NM_VPN_EDITOR_PLUGIN_GET_INTERFACE (plugin);
		if (interface->notify_plugin_info_set)
			interface->notify_plugin_info_set (plugin, plugin_info);
	}

}

/*********************************************************************/

static NMVpnEditorPlugin *
_nm_vpn_editor_plugin_load (const char *plugin_name,
                            gboolean do_file_checks,
                            const char *check_service,
                            int check_owner,
                            NMUtilsCheckFilePredicate check_file,
                            gpointer user_data,
                            GError **error)
{
	void *dl_module = NULL;
	gboolean loaded_before;
	NMVpnEditorPluginFactory factory = NULL;
	gs_unref_object NMVpnEditorPlugin *editor_plugin = NULL;
	gs_free char *plugin_filename_free = NULL;
	const char *plugin_filename;
	gs_free_error GError *factory_error = NULL;
	gs_free char *plug_name = NULL;
	gs_free char *plug_service = NULL;

	g_return_val_if_fail (plugin_name && *plugin_name, NULL);

	/* if @do_file_checks is FALSE, we pass plugin_name directly to
	 * g_module_open().
	 *
	 * Otherwise, we allow for library names without path component.
	 * In which case, we prepend the plugin directory and form an
	 * absolute path. In that case, we perform checks on the file.
	 *
	 * One exception is that we don't allow for the "la" suffix. The
	 * reason is that g_module_open() interprets files with this extension
	 * special and we don't want that. */
	plugin_filename = plugin_name;
	if (do_file_checks) {
		if (   !strchr (plugin_name, '/')
		    && !g_str_has_suffix (plugin_name, ".la")) {
			plugin_filename_free = g_module_build_path (NMPLUGINDIR, plugin_name);
			plugin_filename = plugin_filename_free;
		}
	}

	dl_module = dlopen (plugin_filename, RTLD_LAZY | RTLD_LOCAL | RTLD_NOLOAD);
	if (   !dl_module
	    && do_file_checks) {
		/* If the module is already loaded, we skip the file checks.
		 *
		 * _nm_utils_check_module_file() fails with ENOENT if the plugin file
		 * does not exist. That is relevant, because nm-applet checks for that. */
		if (!_nm_utils_check_module_file (plugin_filename,
		                                  check_owner,
		                                  check_file,
		                                  user_data,
		                                  error))
			return NULL;
	}

	if (dl_module) {
		loaded_before = TRUE;
	} else {
		loaded_before = FALSE;
		dl_module = dlopen (plugin_filename, RTLD_LAZY | RTLD_LOCAL);
	}

	if (!dl_module) {
		g_set_error (error,
		             NM_VPN_PLUGIN_ERROR,
		             NM_VPN_PLUGIN_ERROR_FAILED,
		             _("cannot load plugin \"%s\": %s"),
		             plugin_name,
		             dlerror () ?: "unknown reason");
		return NULL;
	}

	factory = dlsym (dl_module, "nm_vpn_editor_plugin_factory");
	if (!factory) {
		g_set_error (error,
		             NM_VPN_PLUGIN_ERROR,
		             NM_VPN_PLUGIN_ERROR_FAILED,
		             _("failed to load nm_vpn_editor_plugin_factory() from %s (%s)"),
		             plugin_name, dlerror ());
		dlclose (dl_module);
		return NULL;
	}

	editor_plugin = factory (&factory_error);

	if (loaded_before) {
		/* we want to leak the library, because the factory will register glib
		 * types, which cannot be unregistered.
		 *
		 * However, if the library was already loaded before, we want to return
		 * our part of the reference count. */
		dlclose (dl_module);
	}

	if (!editor_plugin) {
		if (factory_error) {
			g_propagate_error (error, factory_error);
			factory_error = NULL;
		} else {
			g_set_error (error,
			             NM_VPN_PLUGIN_ERROR,
			             NM_VPN_PLUGIN_ERROR_FAILED,
			             _("unknown error initializing plugin %s"), plugin_name);
		}
		return NULL;
	}

	g_return_val_if_fail (G_IS_OBJECT (editor_plugin), NULL);

	/* Validate plugin properties */
	g_object_get (G_OBJECT (editor_plugin),
	              NM_VPN_EDITOR_PLUGIN_NAME, &plug_name,
	              NM_VPN_EDITOR_PLUGIN_SERVICE, &plug_service,
	              NULL);

	if (!plug_name || !*plug_name) {
		g_set_error (error,
		             NM_VPN_PLUGIN_ERROR,
		             NM_VPN_PLUGIN_ERROR_FAILED,
		             _("cannot load VPN plugin in '%s': missing plugin name"),
		             plugin_name);
		return NULL;
	}
	if (   check_service
	    && g_strcmp0 (plug_service, check_service) != 0) {
		g_set_error (error,
		             NM_VPN_PLUGIN_ERROR,
		             NM_VPN_PLUGIN_ERROR_FAILED,
		             _("cannot load VPN plugin in '%s': invalid service name"),
		             plugin_name);
		return NULL;
	}

	return g_steal_pointer (&editor_plugin);
}

/**
 * nm_vpn_editor_plugin_load_from_file:
 * @plugin_name: The path or name of the shared library to load.
 *  The path must either be an absolute filename to an existing file.
 *  Alternatively, it can be the name (without path) of a library in the
 *  plugin directory of NetworkManager.
 * @check_service: if not-null, check that the loaded plugin advertises
 *  the given service.
 * @check_owner: if non-negative, check whether the file is owned
 *  by UID @check_owner or by root. In this case also check that
 *  the file is not writable by anybody else.
 * @check_file: (scope call): optional callback to validate the file prior to
 *   loading the shared library.
 * @user_data: user data for @check_file
 * @error: on failure the error reason.
 *
 * Load the shared libary @plugin_name and create a new
 * #NMVpnEditorPlugin instace via the #NMVpnEditorPluginFactory
 * function.
 *
 * If @plugin_name is not an absolute path name, it assumes the file
 * is in the plugin directory of NetworkManager. In any case, the call
 * will do certain checks on the file before passing it to dlopen.
 * A consequence for that is, that you cannot omit the ".so" suffix
 * as you could for nm_vpn_editor_plugin_load().
 *
 * Returns: (transfer full): a new plugin instance or %NULL on error.
 *
 * Since: 1.2
 */
NMVpnEditorPlugin *
nm_vpn_editor_plugin_load_from_file  (const char *plugin_name,
                                      const char *check_service,
                                      int check_owner,
                                      NMUtilsCheckFilePredicate check_file,
                                      gpointer user_data,
                                      GError **error)
{
	return _nm_vpn_editor_plugin_load (plugin_name,
	                                   TRUE,
	                                   check_service,
	                                   check_owner,
	                                   check_file,
	                                   user_data,
	                                   error);
}

/**
 * nm_vpn_editor_plugin_load:
 * @plugin_name: The name of the shared library to load.
 *  This path will be directly passed to dlopen() without
 *  further checks.
 * @check_service: if not-null, check that the loaded plugin advertises
 *  the given service.
 * @error: on failure the error reason.
 *
 * Load the shared libary @plugin_name and create a new
 * #NMVpnEditorPlugin instace via the #NMVpnEditorPluginFactory
 * function.
 *
 * This is similar to nm_vpn_editor_plugin_load_from_file(), but
 * it does no validation of the plugin name, instead passes it directly
 * to dlopen(). If you have the full path to a plugin file,
 * nm_vpn_editor_plugin_load_from_file() is preferred.
 *
 * Returns: (transfer full): a new plugin instance or %NULL on error.
 *
 * Since: 1.4
 */
NMVpnEditorPlugin *
nm_vpn_editor_plugin_load (const char *plugin_name,
                           const char *check_service,
                           GError **error)
{
	return _nm_vpn_editor_plugin_load (plugin_name,
	                                   FALSE,
	                                   check_service,
	                                   -1,
	                                   NULL,
	                                   NULL,
	                                   error);
}

/*********************************************************************/

/**
 * nm_vpn_editor_plugin_get_editor:
 * @plugin: the #NMVpnEditorPlugin
 * @connection: the #NMConnection to be edited
 * @error: on return, an error or %NULL
 *
 * Returns: (transfer full): a new #NMVpnEditor or %NULL on error
 */
NMVpnEditor *
nm_vpn_editor_plugin_get_editor (NMVpnEditorPlugin *plugin,
                                 NMConnection *connection,
                                 GError **error)
{
	g_return_val_if_fail (NM_IS_VPN_EDITOR_PLUGIN (plugin), NULL);

	return NM_VPN_EDITOR_PLUGIN_GET_INTERFACE (plugin)->get_editor (plugin, connection, error);
}

NMVpnEditorPluginCapability
nm_vpn_editor_plugin_get_capabilities (NMVpnEditorPlugin *plugin)
{
	g_return_val_if_fail (NM_IS_VPN_EDITOR_PLUGIN (plugin), 0);

	return NM_VPN_EDITOR_PLUGIN_GET_INTERFACE (plugin)->get_capabilities (plugin);
}

/*****************************************************************************/

static gboolean
_call_fail_not_supported (const char *call_name, GError **error)
{
	g_set_error (error,
	             NM_VPN_PLUGIN_ERROR,
	             NM_VPN_PLUGIN_ERROR_CALL_UNSUPPORTED,
	             _("the plugin does not support call %s"),
	             call_name);
	return FALSE;
}

static gboolean
_call_fail_invalid_args (const char *call_name, GError **error)
{
	/* The plugin determines the signature of the @call_name request.
	 * Caller and plugins should always agree here, this is especially
	 * important to implement nm_vpn_editor_plugin_call() using variadic
	 * arguments.
	 *
	 * Still, in this case the caller is wrong, signal the error. */
	g_set_error (error,
	             NM_VPN_PLUGIN_ERROR,
	             NM_VPN_PLUGIN_ERROR_CALL_INVALID_ARGUMENT,
	             _("invalid arguments for call %s"),
	             call_name);
	return FALSE;
}

/**
 * nm_vpn_editor_plugin_callv:
 * @plugin: the #NMVpnEditorPlugin
 * @call_name: the name of the call_name.
 * @error: (allow-none): an error reason if the call fails.
 * @args_in: (allow-none): a %NULL terminated list of input arguments
 * @args_out: (allow-none): a %NULL terminated list of output arguments
 *
 * Calls a function named @call_name on the plugin and passes input/output
 * arguments.
 *
 * Returns: whether the call was successfull.
 *
 * Since: 1.4
 * */
gboolean
nm_vpn_editor_plugin_callv (NMVpnEditorPlugin *plugin,
                            const char *call_name,
                            GError **error,
                            const GValue *const*args_in,
                            GValue *const*args_out)
{
	NMVpnEditorPluginInterface *interface;
	const GValue *dummy_in = NULL;
	GValue *dummy_out = NULL;
	GType *types_in, *types_out, types_dummy = 0;
	gs_free GType *types_in_free = NULL;
	gs_free GType *types_out_free = NULL;
	gboolean free_types;
	guint i;

	g_return_val_if_fail (NM_IS_VPN_EDITOR_PLUGIN (plugin), FALSE);
	g_return_val_if_fail (call_name, FALSE);
	g_return_val_if_fail (!error || !*error, FALSE);

	interface = NM_VPN_EDITOR_PLUGIN_GET_INTERFACE (plugin);

	if (   !interface->call_get_signature
	    || !interface->call)
		return _call_fail_not_supported (call_name, error);

	/* first check that the signature/types match. We allow that a plugin doesn't understand
	 * a certain request @call_name. But everybody must have the same understanding about the
	 * type signature. Otherwise, the variadic argument parsing in nm_vpn_editor_plugin_call()
	 * cannot work. */

	free_types = FALSE;
	types_in = NULL;
	types_out = NULL;
	if (!interface->call_get_signature (plugin, call_name, &free_types, &types_in, &types_out))
		return _call_fail_not_supported (call_name, error);

	if (free_types) {
		types_in_free = types_in;
		types_out_free = types_out;
	}

	if (!types_in)
		types_in = &types_dummy;
	if (!types_out)
		types_out = &types_dummy;

	if (!args_in)
		args_in = &dummy_in;
	if (!args_out)
		args_out = &dummy_out;

	for (i = 0; types_in[i]; i++) {
		if (!args_in[i] || !G_VALUE_HOLDS (args_in[i], types_in[i]))
			return _call_fail_invalid_args (call_name, error);
	}
	if (args_in[i])
		return _call_fail_invalid_args (call_name, error);

	for (i = 0; types_out[i]; i++) {
		if (!args_out[i] || !G_VALUE_HOLDS (args_out[i], types_out[i]))
			return _call_fail_invalid_args (call_name, error);
	}
	if (args_out[i])
		return _call_fail_invalid_args (call_name, error);

	if (!interface->call (plugin,
	                      call_name,
	                      error,
	                      args_in,
	                      args_out)) {
		if (error && !error) {
			/* for convenience, allow the plugin not to set @error. */
			_call_fail_not_supported (call_name, error);
		}
		return FALSE;
	}

	return TRUE;
}

/**
 * nm_vpn_editor_plugin_call:
 * @plugin: the #NMVpnEditorPlugin
 * @call_name: the name of the call_name.
 * @error: (allow-none): an error reason in case the call fails.
 * @...: a variadic list of input arguments, followed by the output
 *   arguments. For each argument first pass a GType following the value.
 *   A GType of zero terminates the input and output list.
 *
 * Calls a function named @call_name on the plugin and passes input/output
 * arguments.
 *
 * For each @call_name, the signature of the function is determined by the
 * plugin, and the caller must adhere to it. Otherwise, it likely results in
 * a crash. nm_vpn_editor_plugin_callv() is safer here, as it can check the
 * argument types. However, the solution here is that both user and plugins
 * must use the same call signature and the call signature of a once defined call
 * must not change. Everything else is a bug in the plugin which must keep
 * the call signature stable for all VPN plugins.
 *
 * Returns: whether the call was successfull.
 *
 * Since: 1.4
 * */
gboolean
nm_vpn_editor_plugin_call (NMVpnEditorPlugin *plugin,
                           const char *call_name,
                           GError **error,
                           ...)
{
	NMVpnEditorPluginInterface *interface;
	GType *types_in, *types_out, types_dummy = 0;
	gs_free GType *types_in_free = NULL;
	gs_free GType *types_out_free = NULL;
	GType t;
	guint n_in = 0, n_out = 0;
	guint i;
	GValue *args_in, *args_out;
	GValue **args_in_p, **args_out_p;
	va_list ap;
	gboolean success;
	gboolean free_types;
	gboolean invalid_out_types;

	g_return_val_if_fail (NM_IS_VPN_EDITOR_PLUGIN (plugin), FALSE);
	g_return_val_if_fail (call_name, FALSE);
	g_return_val_if_fail (!error || !*error, FALSE);

	interface = NM_VPN_EDITOR_PLUGIN_GET_INTERFACE (plugin);

	if (   !interface->call_get_signature
	    || !interface->call)
		return _call_fail_not_supported (call_name, error);

	free_types = FALSE;
	types_in = NULL;
	types_out = NULL;
	if (!interface->call_get_signature (plugin, call_name, &free_types, &types_in, &types_out))
		return _call_fail_not_supported (call_name, error);

	if (free_types) {
		types_in_free = types_in;
		types_out_free = types_out;
	}

	if (!types_in)
		types_in = &types_dummy;
	if (!types_out)
		types_out = &types_dummy;

	for (n_in = 0; types_in[n_in]; n_in++)
		;
	for (n_out = 0; types_out[n_out]; n_out++)
		;

	args_in = g_newa (GValue, n_in ?: 1);
	args_out = g_newa (GValue, n_out ?: 1);
	args_in_p = g_newa (GValue *, n_in + 1);
	args_out_p = g_newa (GValue *, n_out + 1);
	memset (args_in, 0, sizeof (GValue) * n_in);
	memset (args_out, 0, sizeof (GValue) * n_out);

	va_start (ap, error);

	for (i = 0; TRUE; i++) {
		char *err_msg = NULL;

		t = va_arg (ap, GType);
		if (t != types_in[i]) {
			while (i > 0)
				g_value_unset (&args_in[--i]);
			_call_fail_invalid_args (call_name, error);
			g_return_val_if_reached (FALSE);
		}
		if (t == 0)
			break;

		G_VALUE_COLLECT_INIT (&args_in[i],
		                      types_in[i],
		                      ap,
		                      0,
		                      &err_msg);
		if (err_msg) {
			g_critical ("%s", err_msg);
			g_free (err_msg);

			/* we leak the current argument here, it is not in a sane state. */
			while (i--)
				g_value_unset (&args_in[i]);

			va_end (ap);
			return _call_fail_invalid_args (call_name, error);
		}

		args_in_p[i] = &args_in[i];
	}
	args_in_p[i] = NULL;

	for (i = 0; i < n_out; i++) {
		g_value_init (&args_out[i], types_out[i]);
		args_out_p[i] = &args_out[i];
	}
	args_out_p[i] = NULL;

	success = interface->call (plugin, call_name, error, (const GValue *const*) args_in_p, args_out_p);
	invalid_out_types = FALSE;
	for (i = 0; i < n_out; i++) {
		/* only in case of success we propagate the result back. Otherwise, we
		 * clear every out-argument that the plugin might have set to args_out. */
		if (success) {
			char *err_msg = NULL;

			/* we can check the output arguments only afterwards. The reason is that
			 * we cannot iterate over @ap, because there is no G_VALUE_LCOPY_SKIP.
			 *
			 * Usually, we do not expect failures here, so that shouldn't be too bad. */
			t = va_arg (ap, GType);
			if (t != types_out[i]) {
				g_critical ("mismatch of output type for %s", call_name);
				success = FALSE;
			} else {
				nm_assert (G_VALUE_HOLDS (&args_out[i], t));
				G_VALUE_LCOPY (&args_out[i],
				               ap,
				               0,
				               &err_msg);
				if (err_msg) {
					g_critical ("%s", err_msg);
					g_free (err_msg);
					success = FALSE;
					/* we leak the current value here, it is not in a sane state. */
					continue;
				}
			}
		}
		g_value_unset (&args_out[i]);
	}
	if (success) {
		t = va_arg (ap, GType);
		if (t != 0) {
			g_critical ("mismatch of output type for %s", call_name);
			success = FALSE;
		}
	}
	for (i = 0; i < n_in; i++)
		g_value_unset (&args_in[i]);

	va_end (ap);

	if (!success && error && !error) {
		/* for convenience, allow the plugin not to set @error. */
		_call_fail_not_supported (call_name, error);
	}
	return success;
}

/*****************************************************************************/

/**
 * nm_vpn_editor_plugin_import:
 * @plugin: the #NMVpnEditorPlugin
 * @path: full path to the file to attempt to read into a new #NMConnection
 * @error: on return, an error or %NULL
 *
 * Returns: (transfer full): a new #NMConnection imported from @path, or %NULL
 * on error or if the file at @path was not recognized by this plugin
 */
NMConnection *
nm_vpn_editor_plugin_import (NMVpnEditorPlugin *plugin,
                             const char *path,
                             GError **error)
{
	g_return_val_if_fail (NM_IS_VPN_EDITOR_PLUGIN (plugin), NULL);

	if (nm_vpn_editor_plugin_get_capabilities (plugin) & NM_VPN_EDITOR_PLUGIN_CAPABILITY_IMPORT) {
		g_return_val_if_fail (NM_VPN_EDITOR_PLUGIN_GET_INTERFACE (plugin)->import_from_file != NULL, NULL);
		return NM_VPN_EDITOR_PLUGIN_GET_INTERFACE (plugin)->import_from_file (plugin, path, error);
	}

	g_set_error (error,
	             NM_VPN_PLUGIN_ERROR,
	             NM_VPN_PLUGIN_ERROR_FAILED,
	             _("the plugin does not support import capability"));
	return NULL;
}

gboolean
nm_vpn_editor_plugin_export (NMVpnEditorPlugin *plugin,
                             const char *path,
                             NMConnection *connection,
                             GError **error)
{
	g_return_val_if_fail (NM_IS_VPN_EDITOR_PLUGIN (plugin), FALSE);

	if (nm_vpn_editor_plugin_get_capabilities (plugin) & NM_VPN_EDITOR_PLUGIN_CAPABILITY_EXPORT) {
		g_return_val_if_fail (NM_VPN_EDITOR_PLUGIN_GET_INTERFACE (plugin)->export_to_file != NULL, FALSE);
		return NM_VPN_EDITOR_PLUGIN_GET_INTERFACE (plugin)->export_to_file (plugin, path, connection, error);
	}

	g_set_error (error,
	             NM_VPN_PLUGIN_ERROR,
	             NM_VPN_PLUGIN_ERROR_FAILED,
	             _("the plugin does not support export capability"));
	return FALSE;
}

char *
nm_vpn_editor_plugin_get_suggested_filename (NMVpnEditorPlugin *plugin,
                                             NMConnection *connection)
{
	g_return_val_if_fail (NM_IS_VPN_EDITOR_PLUGIN (plugin), NULL);

	if (NM_VPN_EDITOR_PLUGIN_GET_INTERFACE (plugin)->get_suggested_filename)
		return NM_VPN_EDITOR_PLUGIN_GET_INTERFACE (plugin)->get_suggested_filename (plugin, connection);
	return NULL;
}

