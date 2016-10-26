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

Message *MessagesTable::previousMessage(Message* message, QHash<MessageId, Message *> &messages, QObject *parent) const
{
    TLInt dialogid = TelegramHelper::messageToDialog(message);
    CreateQuery(queryobj);

    if(!this->prepare(queryobj, "SELECT * FROM " + this->name() + " WHERE dialogid = :dialogid AND date < :date ORDER BY date DESC LIMIT 1"))
        return NULL;

    queryobj.bindValue(":dialogid", dialogid);
    queryobj.bindValue(":date", message->date());

    if(!this->execute(queryobj))
        return NULL;

    MessageId prevmessageid = queryobj.value("id").toLongLong();

    if(messages.contains(prevmessageid))
        return messages[prevmessageid];

    if(!queryobj.first())
        return NULL;

    QByteArray data = queryobj.value("message").toByteArray();

    Message* prevmessage = new Message(parent);
    prevmessage->unserialize(data);
    messages[prevmessageid] = prevmessage;
    return prevmessage;
}

QList<Message *> MessagesTable::messagesForDialog(Dialog *dialog, QHash<MessageId, Message *> &messages, int offset, int limit, QObject *parent) const
{
    QList<Message*> result;
    TLInt dialogid = TelegramHelper::identifier(dialog);

    CreateQuery(queryobj);

    if(!this->prepare(queryobj, "SELECT * FROM " + this->name() + " WHERE dialogid = :dialogid ORDER BY date DESC LIMIT :limit OFFSET :offset"))
        return result;

    queryobj.bindValue(":dialogid", dialogid);
    queryobj.bindValue(":limit", limit);
    queryobj.bindValue(":offset", offset);

    if(!this->execute(queryobj))
        return result;

    while(queryobj.next())
    {
        MessageId messageid = queryobj.value("id").toLongLong();

        if(messages.contains(messageid))
        {
            result << messages[messageid];
            continue;
        }

        QByteArray data = queryobj.value("message").toByteArray();
        Message* message = new Message(parent);
        message->unserialize(data);

        messages[messageid] = message;
        result << message;
    }

    return result;
}
