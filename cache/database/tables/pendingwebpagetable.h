#ifndef PENDINGWEBPAGETABLE_H
#define PENDINGWEBPAGETABLE_H

#include "databasetable.h"

class PendingWebPageTable : public DatabaseTable
{
    Q_OBJECT

    public:
        explicit PendingWebPageTable(QObject *parent = 0);
        virtual void createSchema();
        virtual void insertQuery(QSqlQuery &queryobj, TelegramObject *telegramobject);
        MessageId messageId(WebPage* webpage);
};

#endif // PENDINGWEBPAGETABLE_H
