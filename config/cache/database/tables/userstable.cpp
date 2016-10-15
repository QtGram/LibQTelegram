#include "userstable.h"

UsersTable::UsersTable(QObject *parent) : DatabaseTable("users", parent)
{

}

void UsersTable::createSchema()
{
    this->createTable("id INTEGER PRIMARY KEY, iscontact INTEGER, user BLOB", "user");
}

void UsersTable::insertQuery(QSqlQuery &queryobj, TelegramObject *telegramobject)
{
    User* user = qobject_cast<User*>(telegramobject);
    this->prepareInsert(queryobj);

    QByteArray data;
    telegramobject->serialize(data);

    queryobj.bindValue(":id", TelegramHelper::identifier(user));
    queryobj.bindValue(":iscontact", user->isContact());
    queryobj.bindValue(":user", data);

    this->execute(queryobj);
}

void UsersTable::populateContacts(QList<User *> &contacts, QObject *parent) const
{
    CreateQuery(queryobj);

    if(!this->prepare(queryobj, "SELECT * FROM " + this->name() + " WHERE iscontact=1"))
        return;

    this->execute(queryobj);

    while(queryobj.next())
    {
        User* user = new User(parent);
        QByteArray data = queryobj.value("user").toByteArray();

        user->unserialize(data);
        contacts << user;
    }
}
