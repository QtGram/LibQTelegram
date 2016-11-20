#ifndef USERSTABLE_H
#define USERSTABLE_H

#include "databasetable.h"

class UsersTable : public DatabaseTable
{
    Q_OBJECT

    public:
        explicit UsersTable(QObject *parent = 0);
        virtual void createSchema();
        virtual void insertQuery(QSqlQuery& queryobj, TelegramObject *telegramobject);
        void populateContacts(QList<User*>& contacts, QObject* parent) const;
};

#endif // USERSTABLE_H
