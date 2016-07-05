/* -*- Mode: C; tab-width: 4; indent-tabs-mode: t; c-basic-offset: 4 -*- */

#ifndef __NETWORKMANAGER_PACRUNNER_MANAGER_H__
#define __NETWORKMANAGER_PACRUNNER_MANAGER_H__

#include "nm-default.h"

#define NM_TYPE_PACRUNNER_MANAGER            (nm_pacrunner_manager_get_type ())
#define NM_PACRUNNER_MANAGER(obj)            (G_TYPE_CHECK_INSTANCE_CAST ((obj), NM_TYPE_PACRUNNER_MANAGER, NMPacRunnerManager))
#define NM_PACRUNNER_MANAGER_CLASS(klass)    (G_TYPE_CHECK_CLASS_CAST ((klass), NM_TYPE_PACRUNNER_MANAGER, NMPacRunnerManagerClass))
#define NM_IS_PACRUNNER_MANAGER(obj)         (G_TYPE_CHECK_INSTANCE_TYPE ((obj), NM_TYPE_PACRUNNER_MANAGER))
#define NM_IS_PACRUNNER_MANAGER_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE ((klass), NM_TYPE_PACRUNNER_MANAGER))
#define NM_PACRUNNER_MANAGER_GET_CLASS(obj)  (G_TYPE_INSTANCE_GET_CLASS ((obj), NM_TYPE_PACRUNNER_MANAGER, NMPacRunnerManagerClass))

struct _NMPacRunnerManager {
	GObject parent;
};

typedef struct {
	GObjectClass parent;
} NMPacRunnerManagerClass;

GType nm_pacrunner_manager_get_type (void);

NMPacRunnerManager *nm_pacrunner_manager_get (void);

gboolean nm_pacrunner_manager_send (NMPacRunnerManager *self,
                                    const char *iface,
                                    NMProxyConfig *proxy_config,
                                    NMIP4Config *ip4_config,
                                    NMIP6Config *ip6_config);

void nm_pacrunner_manager_remove (NMPacRunnerManager *self, const char *iface);

#endif /* __NETWORKMANAGER_PACRUNNER_MANAGER_H__ */
