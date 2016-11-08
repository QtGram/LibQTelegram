#ifndef DIALOGSTABLE_H
#define DIALOGSTABLE_H

#include "databasetable.h"

class DialogsTable : public DatabaseTable
{
    Q_OBJECT

    public:
        explicit DialogsTable(QObject *parent = 0);
        virtual void createSchema();
        virtual void insertQuery(QSqlQuery &queryobj, TelegramObject *telegramobject);
        void populate(QList<Dialog *> &dialogs, QObject* parent) const;
};

#endif // DIALOGSTABLE_H
