#include "stickersetsdatatable.h"

StickerSetsDataTable::StickerSetsDataTable(QObject *parent) : DatabaseTable("sticker_sets_data", parent)
{

}

void StickerSetsDataTable::createSchema()
{
    this->createTable("id INTEGER PRIMARY KEY, stickerset BLOB", "stickerset");
}

void StickerSetsDataTable::insertQuery(QSqlQuery &queryobj, TelegramObject *telegramobject)
{
    Q_ASSERT(telegramobject->constructorId() == TLTypes::MessagesStickerSet);

    QByteArray data;
    telegramobject->serialize(data);

    MessagesStickerSet* messagesstickerset = qobject_cast<MessagesStickerSet*>(telegramobject);

    queryobj.bindValue(":id", messagesstickerset->set()->id());
    queryobj.bindValue(":stickerset", data);

    this->execute(queryobj);
}
