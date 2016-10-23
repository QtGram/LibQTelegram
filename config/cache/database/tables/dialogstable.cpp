#include "dialogstable.h"

DialogsTable::DialogsTable(QObject *parent) : DatabaseTable("dialogs", parent)
{

}

void DialogsTable::createSchema()
{
    this->createTable("id INTEGER PRIMARY KEY, dialog BLOB", "dialog");
}

void DialogsTable::insertQuery(QSqlQuery &queryobj, TelegramObject *telegramobject)
{
    QByteArray data;
    telegramobject->serialize(data);

    queryobj.bindValue(":id", TelegramHelper::identifier(qobject_cast<Dialog*>(telegramobject)));
    queryobj.bindValue(":dialog", data);

    this->execute(queryobj);
}

void DialogsTable::populate(QList<Dialog *>& dialogs, QObject *parent) const
{
    CreateQuery(queryobj);

    if(!this->query(queryobj, "SELECT * FROM " + this->name()))
        return;

    while(queryobj.next())
    {
        Dialog* dialog = new Dialog(parent);
        QByteArray data = queryobj.value("dialog").toByteArray();

        dialog->unserialize(data);
        dialogs << dialog;
    }
}

bool DialogsTable::contains(TLInt id) const
{
    CreateQuery(queryobj);

    if(!this->prepare(queryobj, "SELECT * FROM " + this->name() + " WHERE id=:id"))
        return false;

    queryobj.bindValue(":id", id);

    if(!this->execute(queryobj))
        return false;

    return queryobj.first();
}
