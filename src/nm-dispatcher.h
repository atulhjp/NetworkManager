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
 * Copyright (C) 2004 - 2012 Red Hat, Inc.
 * Copyright (C) 2005 - 2008 Novell, Inc.
 */

#ifndef __NETWORKMANAGER_DISPATCHER_H__
#define __NETWORKMANAGER_DISPATCHER_H__

#include <stdio.h>

#include "nm-default.h"
#include "nm-connection.h"

typedef enum {
	DISPATCHER_ACTION_HOSTNAME,
	DISPATCHER_ACTION_PRE_UP,
	DISPATCHER_ACTION_UP,
	DISPATCHER_ACTION_PRE_DOWN,
	DISPATCHER_ACTION_DOWN,
	DISPATCHER_ACTION_VPN_PRE_UP,
	DISPATCHER_ACTION_VPN_UP,
	DISPATCHER_ACTION_VPN_PRE_DOWN,
	DISPATCHER_ACTION_VPN_DOWN,
	DISPATCHER_ACTION_DHCP4_CHANGE,
	DISPATCHER_ACTION_DHCP6_CHANGE,
	DISPATCHER_ACTION_CONNECTIVITY_CHANGE
} DispatcherAction;

typedef void (*DispatcherFunc) (guint call_id, gpointer user_data);

gboolean nm_dispatcher_call (DispatcherAction action,
                             NMSettingsConnection *settings_connection,
                             NMConnection *applied_connection,
                             NMDevice *device,
                             DispatcherFunc callback,
                             gpointer user_data,
                             guint *out_call_id);

gboolean nm_dispatcher_call_sync (DispatcherAction action,
                                  NMSettingsConnection *settings_connection,
                                  NMConnection *applied_connection,
                                  NMDevice *device);

gboolean nm_dispatcher_call_vpn (DispatcherAction action,
                                 NMSettingsConnection *settings_connection,
                                 NMConnection *applied_connection,
                                 NMDevice *parent_device,
                                 const char *vpn_iface,
                                 NMProxyConfig *vpn_proxy_config,
                                 NMIP4Config *vpn_ip4_config,
                                 NMIP6Config *vpn_ip6_config,
                                 DispatcherFunc callback,
                                 gpointer user_data,
                                 guint *out_call_id);

gboolean nm_dispatcher_call_vpn_sync (DispatcherAction action,
                                      NMSettingsConnection *settings_connection,
                                      NMConnection *applied_connection,
                                      NMDevice *parent_device,
                                      const char *vpn_iface,
                                      NMProxyConfig *vpn_proxy_config,
                                      NMIP4Config *vpn_ip4_config,
                                      NMIP6Config *vpn_ip6_config);

gboolean nm_dispatcher_call_connectivity (DispatcherAction action,
                                          NMConnectivityState state);

void nm_dispatcher_call_cancel (guint call_id);

void nm_dispatcher_init (void);

#endif /* __NETWORKMANAGER_DISPATCHER_H__ */
