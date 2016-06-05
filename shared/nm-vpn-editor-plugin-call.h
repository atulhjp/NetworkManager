/* -*- Mode: C; tab-width: 4; indent-tabs-mode: t; c-basic-offset: 4 -*- */
/* NetworkManager -- Network link manager
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License along
 * with this program; if not, write to the Free Software Foundation, Inc.,
 * 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
 *
 * Copyright (C) 2015 Red Hat, Inc.
 */

#ifndef __NM_VPN_EDITOR_PLUGIN_CALL_H__
#define __NM_VPN_EDITOR_PLUGIN_CALL_H__

/* This header is an internal, header-only file that can be copied to
 * other projects to call well-known service functions on VPN plugins. */

#include <NetworkManager.h>

/* we make use of otherinternal header files, you need those too. */
#include "gsystem-local-alloc.h"
#include "nm-macros-internal.h"

#ifndef NM_VPN_EDITOR_PLUGIN_CALL_GET_SERVICE_INFO
#define NM_VPN_EDITOR_PLUGIN_CALL_GET_SERVICE_INFO "get-service-info"
#endif

static inline gboolean
nm_vpn_editor_plugin_get_service_info (NMVpnEditorPlugin *plugin,
                                       const char *service_type,
                                       char **out_short_name,
                                       char **out_pretty_name,
                                       char **out_description,
                                       guint *out_flags)
{
	gs_free char *short_name_local = NULL;
	gs_free char *pretty_name_local = NULL;
	gs_free char *description_local = NULL;
	guint flags_local = 0;

	g_return_val_if_fail (NM_IS_VPN_EDITOR_PLUGIN (plugin), FALSE);
	g_return_val_if_fail (service_type, FALSE);

	if (!nm_vpn_editor_plugin_call (plugin,
	                                NM_VPN_EDITOR_PLUGIN_CALL_GET_SERVICE_INFO,
	                                NULL,
	                                G_TYPE_STRING, &service_type,
	                                G_TYPE_INVALID,
	                                G_TYPE_STRING, &short_name_local,
	                                G_TYPE_STRING, &pretty_name_local,
	                                G_TYPE_STRING, &description_local,
	                                G_TYPE_UINT,   out_flags ?: &flags_local,
	                                G_TYPE_INVALID))
		return FALSE;
	NM_SET_OUT (out_short_name, g_steal_pointer (&short_name_local));
	NM_SET_OUT (out_pretty_name, g_steal_pointer (&pretty_name_local));
	NM_SET_OUT (out_description, g_steal_pointer (&description_local));
	return TRUE;
}

#endif /* __NM_VPN_EDITOR_PLUGIN_CALL_H__ */
