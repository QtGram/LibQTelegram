#include "chatuserstable.h"

ChatUsersTable::ChatUsersTable(QObject *parent) : DatabaseTable("chat_users", parent)
{

}

void ChatUsersTable::createSchema()
{
    this->createTable("dialogid INTEGER, userid INTEGER", "dialogid", "PRIMARY KEY(dialogid, userid)");
}

void ChatUsersTable::insertQuery(QSqlQuery &queryobj, TLInt dialogid, TLInt userid)
{
    queryobj.bindValue(":dialogid", dialogid);
    queryobj.bindValue(":userid", userid);
    this->execute(queryobj);
}

void ChatUsersTable::insert(TLInt dialogid, TLInt userid)
{
    CreateQuery(queryobj);
    this->prepareInsert(queryobj);
    this->insertQuery(queryobj, dialogid, userid);
}

void ChatUsersTable::insertQuery(QSqlQuery &queryobj, TelegramObject *telegramobject)
{
    Q_UNUSED(queryobj);
    Q_UNUSED(telegramobject);
    Q_ASSERT(false);
}

void ChatUsersTable::insert(TelegramObject *telegramobject)
{
    Q_UNUSED(telegramobject);
    Q_ASSERT(false);
}
