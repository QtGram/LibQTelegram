#ifndef CLIENTSYNCMANAGER_H
#define CLIENTSYNCMANAGER_H

// Generated by APIGenerator 1.0
// DO NOT EDIT!!!

#include "../config/telegramconfig.h"
#include "../cache/telegramcache.h"
#include "tltypes.h"
#include "types/updatesstate.h"
#include <QObject>
#include "types/updates.h"
#include "types/update.h"

class ClientSyncManager: public QObject
{
	Q_OBJECT

	public:
		static void syncUpdate(Update* u, UpdatesState* clientstate);
		static void syncUpdates(Updates* u, UpdatesState* clientstate);
		static void syncState(UpdatesState* serverstate);
	private:
		static void syncPts(TLInt pts, UpdatesState* clientstate);
		static void syncPts(TLInt pts, Dialog* dialog);
		static void syncQts(TLInt qts, UpdatesState* clientstate);
		static void syncDate(TLInt date, UpdatesState* clientstate);
		static void syncSeq(TLInt seq, UpdatesState* clientstate);
		static void syncUnreadCount(TLInt unreadcount, UpdatesState* clientstate);

};

#endif // CLIENTSYNCMANAGER_H
