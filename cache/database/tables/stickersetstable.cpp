#include "stickersetstable.h"

StickerSetsTable::StickerSetsTable(QObject *parent) : DatabaseTable("sticker_sets", parent)
{

}

void StickerSetsTable::createSchema()
{
    this->createTable("id INTEGER PRIMARY KEY, title TEXT, shortname TEXT, stickerset BLOB", "stickerset");
}

void StickerSetsTable::insertQuery(QSqlQuery &queryobj, TelegramObject *telegramobject)
{
    Q_ASSERT(telegramobject->constructorId() == TLTypes::StickerSet);

    QByteArray data;
    telegramobject->serialize(data);

    StickerSet* stickerset = qobject_cast<StickerSet*>(telegramobject);

    queryobj.bindValue(":id", stickerset->id());
    queryobj.bindValue(":title", stickerset->title().toString());
    queryobj.bindValue(":shortname", stickerset->shortName().toString());
    queryobj.bindValue(":stickerset", data);

    this->execute(queryobj);
}

void StickerSetsTable::populate(QHash<TLLong, StickerSet *> &stickersets, QObject *parent) const
{
    CreateQuery(queryobj);

    if(!this->query(queryobj, "SELECT * FROM " + this->name()))
        return;

    while(queryobj.next())
    {
        StickerSet* stickerset = new StickerSet(parent);
        QByteArray data = queryobj.value("stickerset").toByteArray();

        stickerset->unserialize(data);
        stickersets[stickerset->id()] = stickerset;
    }
}
