#ifndef CHATFULLTABLE_H
#define CHATFULLTABLE_H

#include "databasetable.h"

class ChatFullTable : public DatabaseTable
{
    Q_OBJECT

    public:
        explicit ChatFullTable(QObject *parent = 0);
        virtual void createSchema();
        virtual void insertQuery(QSqlQuery &queryobj, TelegramObject *telegramobject);
};

#endif // CHATFULLTABLE_H
