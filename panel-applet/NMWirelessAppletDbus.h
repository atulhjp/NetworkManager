/* NetworkManager Wireless Applet -- Display wireless access points and allow user control
 *
 * Dan Williams <dcbw@redhat.com>
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
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
 *
 * (C) Copyright 2004 Red Hat, Inc.
 */

#ifndef NM_WIRELESS_APPLET_DBUS_H
#define NM_WIRELESS_APPLET_DBUS_H

#include <dbus/dbus.h>
#include <dbus/dbus-glib.h>
#include "NetworkManager.h"
#include "NMWirelessApplet.h"

/* Return codes for functions that use dbus */
enum
{
	RETURN_SUCCESS = 1,
	RETURN_FAILURE = 0,
	RETURN_NO_NM = -1
};


gpointer			nmwa_dbus_worker				(gpointer user_data);

void				nmwa_dbus_set_device			(DBusConnection *connection, const NetworkDevice *dev,
												const WirelessNetwork *network, NMEncKeyType key_type,
												const char *passphrase);

void				nmwa_dbus_create_network			(DBusConnection *connection, const NetworkDevice *dev,
												const WirelessNetwork *network, NMEncKeyType key_type,
												const char *passphrase);

void				nmwa_dbus_enable_scanning		(NMWirelessApplet *applet, gboolean enabled);

void				nmwa_dbus_enable_wireless		(NMWirelessApplet *applet, gboolean enabled);

WirelessNetwork *	wireless_network_new_with_essid	(const char *essid);
void				wireless_network_unref			(WirelessNetwork *net);

void				network_device_ref				(NetworkDevice *dev);
void				network_device_unref			(NetworkDevice *dev);

#endif
