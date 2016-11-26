#ifndef CACHEDATABASE_H
#define CACHEDATABASE_H

#include <QObject>
#include <QSqlDatabase>
#include "tables/chatfulltable.h"
#include "tables/chatstable.h"
#include "tables/dialogstable.h"
#include "tables/messagestable.h"
#include "tables/migrateddialogstable.h"
#include "tables/pendingwebpagetable.h"
#include "tables/userstable.h"

class CacheDatabase : public QObject
{
    Q_OBJECT

    public:
        explicit CacheDatabase(const QString& configpath, QObject *parent = 0);
        ~CacheDatabase();
        ChatFullTable *chatFull() const;
        ChatsTable* chats() const;
        DialogsTable* dialogs() const;
        MessagesTable* messages() const;
        MigratedDialogsTable* migratedDialogs() const;
        PendingWebPageTable* pendingWebPages() const;
        UsersTable* users() const;
        void transaction(std::function<void(QSqlQuery& queryobj)> transactionproc);

    private:
        void loadTables();

    private:
        ChatFullTable* _chatfulltable;
        ChatsTable* _chatstable;
        DialogsTable* _dialogstable;
        MessagesTable* _messagestable;
        MigratedDialogsTable* _migrateddialogstable;
        PendingWebPageTable* _pendingwebpagestable;
        UsersTable* _userstable;
};

#endif // CACHEDATABASE_H
