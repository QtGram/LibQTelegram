#include "migrateddialogstable.h"

MigratedDialogsTable::MigratedDialogsTable(QObject *parent) : DatabaseTable("migrated_dialogs", parent)
{

}

void MigratedDialogsTable::createSchema()
{
    this->createTable("id INTEGER, dialogid INTEGER", "id", "PRIMARY KEY(id, dialogid)");
}

void MigratedDialogsTable::insertQuery(QSqlQuery &queryobj, TelegramObject *telegramobject)
{
    QByteArray data;
    telegramobject->serialize(data);

    if(telegramobject->constructorId() == TLTypes::MessageService)
    {
        Message* message = qobject_cast<Message*>(telegramobject);
        Q_ASSERT(TelegramHelper::messageIsMigratedTo(message));

        queryobj.bindValue(":id", TelegramHelper::messageToDialog(message));
        queryobj.bindValue(":dialogid", message->action()->channelId());
    }
    else if(telegramobject->constructorId() == TLTypes::Chat)
    {
        Chat* chat = qobject_cast<Chat*>(telegramobject);
        TLInt channelid = TelegramHelper::chatIsMigrated(chat);
        Q_ASSERT(channelid != 0);

        queryobj.bindValue(":id", chat->id());
        queryobj.bindValue(":dialogid", channelid);
    }
    else
        Q_ASSERT(false);

    this->execute(queryobj);
}

void MigratedDialogsTable::populate(QHash<TLInt, TLInt> &migrateddialogs) const
{
    CreateQuery(queryobj);

    if(!this->query(queryobj, "SELECT * FROM " + this->name()))
        return;

    while(queryobj.next())
    {
        TLInt id = queryobj.value("id").toInt();
        TLInt dialogid = queryobj.value("dialogid").toInt();

        migrateddialogs[id] = dialogid;
    }
}
