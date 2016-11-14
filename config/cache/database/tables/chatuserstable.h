#ifndef CHATUSERSTABLE_H
#define CHATUSERSTABLE_H

#include "databasetable.h"

class ChatUsersTable : public DatabaseTable
{
    Q_OBJECT

    public:
        explicit ChatUsersTable(QObject *parent = 0);
        virtual void createSchema();
        virtual void insertQuery(QSqlQuery &queryobj, TLInt dialogid, TLInt userid);
        virtual void insert(TLInt dialogid, TLInt userid);

    private:
        virtual void insertQuery(QSqlQuery &queryobj, TelegramObject *telegramobject);
        virtual void insert(TelegramObject* telegramobject);
};

#endif // CHATUSERSTABLE_H
