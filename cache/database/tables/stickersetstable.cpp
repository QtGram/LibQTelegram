#include "stickersetstable.h"

StickerSetsTable::StickerSetsTable(QObject *parent) : DatabaseTable("sticker_sets", parent)
{

}

void StickerSetsTable::createSchema()
{
    this->createTable("id INTEGER PRIMARY KEY, position INTEGER, title TEXT, shortname TEXT, stickerset BLOB", "stickerset");
}

void StickerSetsTable::insertQuery(QSqlQuery &queryobj, TelegramObject *telegramobject)
{
    Q_ASSERT(telegramobject->constructorId() == TLTypes::StickerSet);

    QByteArray data;
    telegramobject->serialize(data);

    StickerSet* stickerset = qobject_cast<StickerSet*>(telegramobject);

    queryobj.bindValue(":id", stickerset->id());
    queryobj.bindValue(":position", -1);
    queryobj.bindValue(":title", stickerset->title());
    queryobj.bindValue(":shortname", stickerset->shortName());
    queryobj.bindValue(":stickerset", data);

    this->execute(queryobj);
}

void StickerSetsTable::orderQuery(QSqlQuery &queryobj, StickerSet *stickerset, int index)
{
    Q_ASSERT(stickerset != NULL);

    queryobj.bindValue(":id", stickerset->id());
    queryobj.bindValue(":position", index);

    this->execute(queryobj);
}

void StickerSetsTable::order(StickerSet *stickerset, int index)
{
    CreateQuery(queryobj);
    this->prepareOrder(queryobj);
    this->orderQuery(queryobj, stickerset, index);
}

void StickerSetsTable::populate(QHash<TLLong, StickerSet *> &stickersets, QList<StickerSet*>& stickers, QObject *parent) const
{
    CreateQuery(queryobj);

    if(!this->query(queryobj, "SELECT * FROM " + this->name() + " ORDER BY position ASC"))
        return;

    while(queryobj.next())
    {
        StickerSet* stickerset = new StickerSet(parent);
        QByteArray data = queryobj.value("stickerset").toByteArray();

        stickerset->unserialize(data);
        stickersets[stickerset->id()] = stickerset;
        stickers << stickerset;
    }
}

void StickerSetsTable::prepareOrder(QSqlQuery &orderquery)
{
    this->prepare(orderquery, "UPDATE " + this->name() + " SET position=:position WHERE id=:id");
}
