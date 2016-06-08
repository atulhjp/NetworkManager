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
 * Copyright 2008 - 2015 Red Hat, Inc.
 */

#ifndef __NM_VPN_EDITOR_PLUGIN_H__
#define __NM_VPN_EDITOR_PLUGIN_H__

#if !defined (__NETWORKMANAGER_H_INSIDE__) && !defined (NETWORKMANAGER_COMPILATION)
#error "Only <NetworkManager.h> can be included directly."
#endif

#include <glib.h>
#include <glib-object.h>

#include "nm-connection.h"
#include "nm-utils.h"

G_BEGIN_DECLS

typedef struct _NMVpnEditorPlugin NMVpnEditorPlugin;
typedef struct _NMVpnEditor NMVpnEditor;

/* Plugin's factory function that returns a GObject that implements
 * NMVpnEditorPlugin.
 */
#ifndef __GI_SCANNER__
typedef NMVpnEditorPlugin * (*NMVpnEditorPluginFactory) (GError **error);
NMVpnEditorPlugin *nm_vpn_editor_plugin_factory (GError **error);
#endif


/**************************************************/
/* Editor plugin interface                        */
/**************************************************/

#define NM_TYPE_VPN_EDITOR_PLUGIN               (nm_vpn_editor_plugin_get_type ())
#define NM_VPN_EDITOR_PLUGIN(obj)               (G_TYPE_CHECK_INSTANCE_CAST ((obj), NM_TYPE_VPN_EDITOR_PLUGIN, NMVpnEditorPlugin))
#define NM_IS_VPN_EDITOR_PLUGIN(obj)            (G_TYPE_CHECK_INSTANCE_TYPE ((obj), NM_TYPE_VPN_EDITOR_PLUGIN))
#define NM_VPN_EDITOR_PLUGIN_GET_INTERFACE(obj) (G_TYPE_INSTANCE_GET_INTERFACE ((obj), NM_TYPE_VPN_EDITOR_PLUGIN, NMVpnEditorPluginInterface))

/**
 * NMVpnEditorPluginCapability:
 * @NM_VPN_EDITOR_PLUGIN_CAPABILITY_NONE: unknown or no capability
 * @NM_VPN_EDITOR_PLUGIN_CAPABILITY_IMPORT: the plugin can import new connections
 * @NM_VPN_EDITOR_PLUGIN_CAPABILITY_EXPORT: the plugin can export connections
 * @NM_VPN_EDITOR_PLUGIN_CAPABILITY_IPV6: the plugin supports IPv6 addressing
 *
 * Flags that indicate certain capabilities of the plugin to editor programs.
 **/
typedef enum /*< flags >*/ {
	NM_VPN_EDITOR_PLUGIN_CAPABILITY_NONE   = 0x00,
	NM_VPN_EDITOR_PLUGIN_CAPABILITY_IMPORT = 0x01,
	NM_VPN_EDITOR_PLUGIN_CAPABILITY_EXPORT = 0x02,
	NM_VPN_EDITOR_PLUGIN_CAPABILITY_IPV6   = 0x04
} NMVpnEditorPluginCapability;

/* Short display name of the VPN plugin */
#define NM_VPN_EDITOR_PLUGIN_NAME "name"

/* Longer description of the VPN plugin */
#define NM_VPN_EDITOR_PLUGIN_DESCRIPTION "description"

/* D-Bus service name of the plugin's VPN service */
#define NM_VPN_EDITOR_PLUGIN_SERVICE "service"


/**
 * NMVpnEditorPluginServiceFlags:
 * @NM_VPN_EDITOR_PLUGIN_SERVICE_FLAGS_NONE: no flags
 * @NM_VPN_EDITOR_PLUGIN_SERVICE_FLAGS_CAN_ADD: whether the plugin can
 *   add a new connection for the given service-type.
 **/
typedef enum { /*< skip >*/
	NM_VPN_EDITOR_PLUGIN_SERVICE_FLAGS_NONE     = 0x00,
	NM_VPN_EDITOR_PLUGIN_SERVICE_FLAGS_CAN_ADD  = 0x01,
} NMVpnEditorPluginServiceFlags;

/**
 * NM_VPN_EDITOR_PLUGIN_CALL_GET_SERVICE_INFO: name of call "get-service-info".
 *   This call has 1 input argument:
 *     service-type (string): analog to NM_VPN_EDITOR_PLUGIN_SERVICE
 *     it specifies for which service-type the information is requested.
 *     This can either be the main service-type or an alias.
 *   This call has 3 output arguments:
 *     short-name (string): for the main service-type, this shall return [VPN Connection].name.
 *        Otherwise, it is a short-name to refer to service-type.
 *     pretty-name (string): for the main service-type, this shall return NM_VPN_EDITOR_PLUGIN_NAME.
 *        It's a localized, pretty name of the service-type.
 *     description (string): for the main service-type, this shall return NM_VPN_EDITOR_PLUGIN_DESCRIPTION
 *        It's a localized, description for the service-type.
 *     service flags (uint): flags for the service-type.
 */
#define NM_VPN_EDITOR_PLUGIN_CALL_GET_SERVICE_INFO "get-service-info"

/**
 * NM_VPN_EDITOR_PLUGIN_CALL_GET_SERVICE_ADD_DETAILS: name of call "get-service-add-details".
 *   For one particular service-type, the UI might want to show multiple "Add new connection"
 *   entires. That is controlled by passing around "add-details".
 *   The "get-service-add-details" returns optionally a list of "add-details" if it wishes
 *   to generate multiple add entries.
 *   This call has 1 input argument:
 *     service-type: string: analog to NM_VPN_EDITOR_PLUGIN_SERVICE or an service-type
 *        alias.
 *   This call has 1 output argument:
 *     add-details: strv: a list of details that can be passed to "get-service-add-detail"
 *        call.
 */
#define NM_VPN_EDITOR_PLUGIN_CALL_GET_SERVICE_ADD_DETAILS "get-service-add-details"

/**
 * NM_VPN_EDITOR_PLUGIN_CALL_GET_SERVICE_ADD_DETAIL: if the plugin supports "add-details"
 *   as indicated by "get-service-add-details", return more information.
 *   This call has 2 input arguments:
 *     service-name: string: analog to NM_VPN_EDITOR_PLUGIN_SERVICE or an service-type
 *        alias. This was passed to "get-service-add-details" call.
 *     add-detail: a detail for which the information is requested.
 *   This call has 1 output argument:
 *     pretty-name: (string), a localized name for what is to be added. Similar to
 *       NM_VPN_EDITOR_PLUGIN_NAME.
 *     description: (string), a localized descirption, similar to NM_VPN_EDITOR_PLUGIN_DESCRIPTION.
 *     add-detail-key: (string), when creating such a connection of type "service-type","add-detail",
 *       the user shall create a NMConnection with setting "service-type". It also sets the VPN
 *       key "add-detail-key" to the value "add-detail", so that the plugin knows which connection
 *       is to be created.
 *     flags: (uint), additional flags, currently unused.
 */
#define NM_VPN_EDITOR_PLUGIN_CALL_GET_SERVICE_ADD_DETAIL "get-service-add-detail"

/**
 * NMVpnEditorPluginInterface:
 * @g_iface: the parent interface
 * @get_editor: returns an #NMVpnEditor, pre-filled with values from @connection
 *   if non-%NULL.
 * @get_capabilities: returns a bitmask of capabilities.
 * @import_from_file: Try to import a connection from the specified path.  On
 *   success, return a partial #NMConnection object.  On error, return %NULL and
 *   set @error with additional information.  Note that @error can be %NULL, in
 *   which case no additional error information should be provided.
 * @export_to_file: Export the given connection to the specified path.  Return
 *   %TRUE on success.  On error, return %FALSE and set @error with additional
 *   error information.  Note that @error can be %NULL, in which case no
 *   additional error information should be provided.
 * @get_suggested_filename: For a given connection, return a suggested file
 *   name.  Returned value will be %NULL or a suggested file name to be freed by
 *   the caller.
 * @call_get_signature: to implement the variadic arguments for nm_vpn_editor_plugin_call(),
 *   the plugin must announce the signature of a call.
 * @call: implementation of nm_vpn_editor_plugin_callv() to call into the plugin.
 *
 * Interface for VPN editor plugins.
 */
typedef struct {
	GTypeInterface g_iface;

	NMVpnEditor * (*get_editor) (NMVpnEditorPlugin *plugin,
	                             NMConnection *connection,
	                             GError **error);

	NMVpnEditorPluginCapability (*get_capabilities) (NMVpnEditorPlugin *plugin);

	NMConnection * (*import_from_file) (NMVpnEditorPlugin *plugin,
	                                    const char *path,
	                                    GError **error);

	gboolean (*export_to_file) (NMVpnEditorPlugin *plugin,
	                            const char *path,
	                            NMConnection *connection,
	                            GError **error);

	char * (*get_suggested_filename) (NMVpnEditorPlugin *plugin, NMConnection *connection);

	gboolean (*call_get_signature) (NMVpnEditorPlugin *plugin,
	                                const char *call_name,
	                                gboolean *free_types,
	                                GType **types_in,
	                                GType **types_out);

	gboolean (*call) (NMVpnEditorPlugin *plugin,
	                  const char *call_name,
	                  GError **error,
	                  const GValue *const*args_in,
	                  GValue *const*args_out);

} NMVpnEditorPluginInterface;

GType nm_vpn_editor_plugin_get_type (void);

NMVpnEditor *nm_vpn_editor_plugin_get_editor (NMVpnEditorPlugin *plugin,
                                              NMConnection *connection,
                                              GError **error);

NMVpnEditorPluginCapability nm_vpn_editor_plugin_get_capabilities (NMVpnEditorPlugin *plugin);

NMConnection *nm_vpn_editor_plugin_import                 (NMVpnEditorPlugin *plugin,
                                                           const char *path,
                                                           GError **error);
gboolean      nm_vpn_editor_plugin_export                 (NMVpnEditorPlugin *plugin,
                                                           const char *path,
                                                           NMConnection *connection,
                                                           GError **error);
char         *nm_vpn_editor_plugin_get_suggested_filename (NMVpnEditorPlugin *plugin,
                                                           NMConnection *connection);

NM_AVAILABLE_IN_1_4
gboolean      nm_vpn_editor_plugin_callv (NMVpnEditorPlugin *plugin,
                                          const char *call_name,
                                          GError **error,
                                          const GValue *const*args_in,
                                          GValue *const*args_out);

NM_AVAILABLE_IN_1_4
gboolean      nm_vpn_editor_plugin_call (NMVpnEditorPlugin *plugin,
                                         const char *call_name,
                                         GError **error,
                                         ...);

NM_AVAILABLE_IN_1_2
NMVpnEditorPlugin *nm_vpn_editor_plugin_load_from_file  (const char *plugin_name,
                                                         const char *check_service,
                                                         int check_owner,
                                                         NMUtilsCheckFilePredicate check_file,
                                                         gpointer user_data,
                                                         GError **error);

NM_AVAILABLE_IN_1_4
NMVpnEditorPlugin *nm_vpn_editor_plugin_load (const char *plugin_name,
                                              const char *check_service,
                                              GError **error);

G_END_DECLS

#endif	/* __NM_VPN_EDITOR_PLUGIN_H__ */
