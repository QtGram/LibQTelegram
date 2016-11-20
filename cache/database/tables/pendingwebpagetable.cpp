#include "pendingwebpagetable.h"

PendingWebPageTable::PendingWebPageTable(QObject *parent) : DatabaseTable("pending_webpages", parent)
{

}

void PendingWebPageTable::createSchema()
{
    this->createTable("id INTEGER PRIMARY KEY, messageid INTEGER", "messageid");
}

void PendingWebPageTable::insertQuery(QSqlQuery &queryobj, TelegramObject *telegramobject)
{
    Message* message = qobject_cast<Message*>(telegramobject);

    if(!TelegramHelper::messageIsWebPagePending(message))
        return;

    WebPage* webpage = message->media()->webpage();

    queryobj.bindValue(":id", webpage->id());
    queryobj.bindValue(":messageid", TelegramHelper::identifier(message));

    this->execute(queryobj);
}

MessageId PendingWebPageTable::messageId(WebPage *webpage)
{
    if(!webpage)
        return 0;

    CreateQuery(queryobj);

    if(!this->prepare(queryobj, "SELECT messageid FROM " + this->name() + " WHERE id=:id"))
        return 0;

    queryobj.bindValue(":id", webpage->id());

    if(!this->execute(queryobj) || !queryobj.first())
        return 0;

    return queryobj.value("messageid").toULongLong();
}
