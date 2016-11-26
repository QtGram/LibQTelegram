#ifndef MIGRATEDDIALOGSTABLE_H
#define MIGRATEDDIALOGSTABLE_H

#include "databasetable.h"

class MigratedDialogsTable : public DatabaseTable
{
    Q_OBJECT
    \
    public:
        explicit MigratedDialogsTable(QObject *parent = 0);
        virtual void createSchema();
        virtual void insertQuery(QSqlQuery &queryobj, TelegramObject *telegramobject);
        void populate(QHash<TLInt, TLInt> &migrateddialogs) const;
};

#endif // MIGRATEDDIALOGSTABLE_H
