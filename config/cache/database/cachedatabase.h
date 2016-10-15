#ifndef CACHEDATABASE_H
#define CACHEDATABASE_H

#include <QObject>
#include <QSqlDatabase>
#include "tables/chatstable.h"
#include "tables/dialogstable.h"
#include "tables/messagestable.h"
#include "tables/userstable.h"

class CacheDatabase : public QObject
{
    Q_OBJECT

    public:
        explicit CacheDatabase(const QString& configpath, QObject *parent = 0);
        ~CacheDatabase();
        ChatsTable* chats() const;
        DialogsTable* dialogs() const;
        MessagesTable* messages() const;
        UsersTable* users() const;
        void transaction(std::function<void(QSqlQuery& queryobj)> transactionproc);

    private:
        void loadTables();

    private:
        ChatsTable* _chatstable;
        DialogsTable* _dialogstable;
        MessagesTable* _messagestable;
        UsersTable* _userstable;
};

#endif // CACHEDATABASE_H
