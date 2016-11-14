#include "chatfulltable.h"

ChatFullTable::ChatFullTable(QObject *parent) : DatabaseTable("chat_full", parent)
{

}

void ChatFullTable::createSchema()
{
    this->createTable("id INTEGER, constructorid INTEGER, ", "messageid");
}

void ChatFullTable::insertQuery(QSqlQuery &queryobj, TelegramObject *telegramobject)
{

}
