#ifndef STICKERSETTABLE_H
#define STICKERSETTABLE_H

#include "databasetable.h"

class StickerSetsTable : public DatabaseTable
{
    Q_OBJECT

    public:
        explicit StickerSetsTable(QObject *parent = 0);
        virtual void createSchema();
        virtual void insertQuery(QSqlQuery &queryobj, TelegramObject *telegramobject);
        void populate(QHash<TLLong, StickerSet *> &stickersets, QObject* parent) const;

};

#endif // STICKERSETTABLE_H
