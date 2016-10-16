#include "messagestable.h"

MessagesTable::MessagesTable(QObject *parent) : DatabaseTable("messages", parent)
{

}

void MessagesTable::createSchema()
{
    this->createTable("id INTEGER PRIMARY KEY, dialogid INTEGER, date INTEGER, message BLOB", "message");
}

void MessagesTable::insertQuery(QSqlQuery &queryobj, TelegramObject *telegramobject)
{
    Message* message = qobject_cast<Message*>(telegramobject);

    QByteArray data;
    message->serialize(data);

    queryobj.bindValue(":id", TelegramHelper::identifier(message));
    queryobj.bindValue(":dialogid", TelegramHelper::messageToDialog(message));
    queryobj.bindValue(":date", message->date());
    queryobj.bindValue(":message", data);

    this->execute(queryobj);
}

Message *MessagesTable::previousMessage(Message* message, QHash<TLInt, Message *> &messages, QObject *parent)
{
    TLInt dialogid = TelegramHelper::messageToDialog(message);
    CreateQuery(queryobj);

    if(!this->prepare(queryobj, "SELECT * FROM " + this->name() + " WHERE dialogid = :dialogid AND date < :date ORDER BY date DESC LIMIT 1"))
        return NULL;

    queryobj.bindValue(":dialogid", dialogid);
    queryobj.bindValue(":date", message->date());

    if(!this->execute(queryobj))
        return NULL;

    TLInt prevmessageid = queryobj.value("id").toInt();

    if(messages.contains(prevmessageid))
        return messages[prevmessageid];

    QByteArray data = queryobj.value("message").toByteArray();

    Message* prevmessage = new Message(parent);
    prevmessage->unserialize(data);
    messages[prevmessageid] = prevmessage;
    return prevmessage;
}

QList<Message *> MessagesTable::messagesForDialog(Dialog *dialog, QHash<TLInt, Message *> &messages, int limit, QObject *parent)
{
    QList<Message*> result;
    TLInt dialogid = TelegramHelper::identifier(dialog);

    CreateQuery(queryobj);

    if(!this->prepare(queryobj, "SELECT * FROM " + this->name() + " WHERE dialogid = :dialogid ORDER BY date DESC LIMIT :limit"))
        return result;

    queryobj.bindValue(":dialogid", dialogid);
    queryobj.bindValue(":limit", limit);

    if(!this->execute(queryobj))
        return result;

    while(queryobj.next())
    {
        TLInt id = queryobj.value("id").toInt();

        if(messages.contains(id))
        {
            result << messages[id];
            continue;
        }

        QByteArray data = queryobj.value("message").toByteArray();
        Message* message = new Message(parent);
        message->unserialize(data);

        messages[message->id()] = message;
        result << message;
    }

    return result;
}
