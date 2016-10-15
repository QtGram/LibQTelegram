#include "chatstable.h"

ChatsTable::ChatsTable(QObject *parent) : DatabaseTable("chats", parent)
{

}

void ChatsTable::createSchema()
{
    this->createTable("id INTEGER PRIMARY KEY, chat BLOB", "chat");
}

void ChatsTable::insertQuery(QSqlQuery &queryobj, TelegramObject *telegramobject)
{
    this->prepareInsert(queryobj);

    QByteArray data;
    telegramobject->serialize(data);

    queryobj.bindValue(":id", TelegramHelper::identifier(qobject_cast<Chat*>(telegramobject)));
    queryobj.bindValue(":chat", data);

    this->execute(queryobj);
}
