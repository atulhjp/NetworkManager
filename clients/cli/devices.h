/* nmcli - command-line tool to control NetworkManager
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
 * (C) Copyright 2010 Red Hat, Inc.
 */

#ifndef NMC_DEVICES_H
#define NMC_DEVICES_H

#include "nmcli.h"

NMCResultCode do_devices (NmCli *nmc, int argc, char **argv);

void nmc_complete_device (NMClient *client, const char *prefix, gboolean wifi_only);

void nmc_complete_bssid (NMClient *client, const char *ifname, const char *bssid_prefix);

void monitor_devices (NmCli *nmc);

NMDevice ** nmc_get_devices_sorted (NMClient *client);

void nmc_device_state_to_color (NMDeviceState state, NmcTermColor *color, NmcTermFormat *color_fmt);

extern NmcOutputField nmc_fields_dev_status[];
extern NmcOutputField nmc_fields_dev_show_general[];
extern NmcOutputField nmc_fields_dev_show_connections[];
extern NmcOutputField nmc_fields_dev_show_cap[];
extern NmcOutputField nmc_fields_dev_show_wired_prop[];
extern NmcOutputField nmc_fields_dev_show_wifi_prop[];
extern NmcOutputField nmc_fields_dev_show_wimax_prop[];
extern NmcOutputField nmc_fields_dev_wifi_list[];
extern NmcOutputField nmc_fields_dev_wimax_list[];
extern NmcOutputField nmc_fields_dev_show_master_prop[];
extern NmcOutputField nmc_fields_dev_show_team_prop[];
extern NmcOutputField nmc_fields_dev_show_vlan_prop[];
extern NmcOutputField nmc_fields_dev_show_bluetooth[];
extern NmcOutputField nmc_fields_dev_show_sections[];
extern NmcOutputField nmc_fields_dev_lldp_list[];

#endif /* NMC_DEVICES_H */
