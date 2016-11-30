#ifndef STICKERSETTABLE_H
#define STICKERSETTABLE_H

#include "databasetable.h"

class StickerSetsTable : public DatabaseTable
{
    Q_OBJECT

    public:
        explicit StickerSetsTable(QObject *parent = 0);
        virtual void createSchema();
        virtual void insertQuery(QSqlQuery& queryobj, TelegramObject* telegramobject);
        void populate(QHash<TLLong, StickerSet *> &stickersets, QList<StickerSet *> &stickers, QObject* parent) const;

    public:
        void prepareOrder(QSqlQuery& orderquery);
        virtual void orderQuery(QSqlQuery& queryobj, StickerSet* stickerset, int index);
        virtual void order(StickerSet* stickerset, int index);

};

#endif // STICKERSETTABLE_H
