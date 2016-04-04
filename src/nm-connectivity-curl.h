/* -*- Mode: C; tab-width: 4; indent-tabs-mode: t; c-basic-offset: 4 -*- */
/*
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
 * Copyright (C) 2016 Red Hat, Inc.
 *
 */

#include <curl/curl.h>

typedef struct {
	CURLM *curl_mhandle;
	guint curl_timer;
	gboolean initial_check_obsoleted;
	guint check_id;
} NMConnectivityConcheck;

typedef struct {
	GSimpleAsyncResult *simple;
	char *uri;
	char *response;
	guint check_id_when_scheduled;
	CURL *curl_ehandle;
	size_t msg_size;
	char *msg;
	curl_socket_t sock;
	GIOChannel *ch;
	guint ev;
} ConCheckCbData;

typedef struct {
	curl_socket_t sockfd;
	CURL *easy;
	int action;
	long timeout;
	GIOChannel *ch;
	guint ev;
} CurlSockData;

