#include "chatfulltable.h"

ChatFullTable::ChatFullTable(QObject *parent) : DatabaseTable("chat_full", parent)
{

}

void ChatFullTable::createSchema()
{
    this->createTable("id INTEGER PRIMARY KEY, chatfull BLOB, pinnedmessageid INTEGER", "chatfull");
}

void ChatFullTable::insertQuery(QSqlQuery &queryobj, TelegramObject *telegramobject)
{
    ChatFull* chatfull = qobject_cast<ChatFull*>(telegramobject);

    QByteArray data;
    chatfull->serialize(data);

    queryobj.bindValue(":id", chatfull->id());
    queryobj.bindValue(":chatfull", data);
    queryobj.bindValue(":pinnedmessageid", chatfull->pinnedMsgId());

    this->execute(queryobj);
}
