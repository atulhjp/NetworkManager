/* NetworkManager -- Network link manager
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

#ifndef NETWORK_MANAGER_AP_LIST_H
#define NETWORK_MANAGER_AP_LIST_H

#include <glib.h>
#include "NetworkManager.h"
#include "NetworkManagerMain.h"
#include "NetworkManagerAP.h"

typedef struct NMAccessPointList	NMAccessPointList;
typedef struct NMAPListIter		NMAPListIter;

NMAccessPointList *	nm_ap_list_new					(NMNetworkType type);
void				nm_ap_list_ref					(NMAccessPointList *list);
void				nm_ap_list_unref				(NMAccessPointList *list);

guint			nm_ap_list_size				(NMAccessPointList *list);
gboolean			nm_ap_list_is_empty				(NMAccessPointList *list);

void				nm_ap_list_append_ap			(NMAccessPointList *list, NMAccessPoint *ap);
void				nm_ap_list_remove_ap			(NMAccessPointList *list, NMAccessPoint *ap);
void				nm_ap_list_remove_ap_by_ssid		(NMAccessPointList *list, const GByteArray * ssid);
void				nm_ap_list_remove_duplicate_ssids	(NMAccessPointList *list);

NMAccessPoint *	nm_ap_list_get_ap_by_ssid		(NMAccessPointList *list, const GByteArray * ssid);
NMAccessPoint *	nm_ap_list_get_ap_by_address		(NMAccessPointList *list, const struct ether_addr *addr);

void				nm_ap_list_copy_properties		(NMAccessPointList *dest, NMAccessPointList *source);
void				nm_ap_list_copy_ssids_by_address	(NMAccessPointList *dest, NMAccessPointList *source);
void				nm_ap_list_copy_one_ssid_by_address	(NMAccessPoint *ap, NMAccessPointList *search_list);

NMNetworkType		nm_ap_list_get_type				(NMAccessPointList *list);

NMAPListIter *		nm_ap_list_iter_new				(NMAccessPointList *list);
NMAccessPoint *	nm_ap_list_iter_get_ap			(NMAPListIter *iter);
NMAccessPoint *	nm_ap_list_iter_next			(NMAPListIter *iter);
void				nm_ap_list_iter_free			(NMAPListIter *iter);

void				nm_ap_list_print_members			(NMAccessPointList *list, const char *name);

#endif
