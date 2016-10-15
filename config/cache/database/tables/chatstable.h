#ifndef CHATSTABLE_H
#define CHATSTABLE_H

#include "databasetable.h"

class ChatsTable : public DatabaseTable
{
    Q_OBJECT

    public:
        explicit ChatsTable(QObject *parent = 0);
        virtual void createSchema();
        virtual void insertQuery(QSqlQuery &queryobj, TelegramObject *telegramobject);
};

#endif // CHATSTABLE_H
