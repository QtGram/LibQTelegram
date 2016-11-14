#include "chatstable.h"

ChatsTable::ChatsTable(QObject *parent) : DatabaseTable("chats", parent)
{

}

void ChatsTable::createSchema()
{
    this->createTable("id INTEGER PRIMARY KEY, title TEXT, chat BLOB", "chat");
}

void ChatsTable::insertQuery(QSqlQuery &queryobj, TelegramObject *telegramobject)
{
    QByteArray data;
    telegramobject->serialize(data);

    Chat* chat = qobject_cast<Chat*>(telegramobject);

    queryobj.bindValue(":id", TelegramHelper::identifier(chat));
    queryobj.bindValue(":title", chat->title().toString());
    queryobj.bindValue(":chat", data);

    this->execute(queryobj);
}
