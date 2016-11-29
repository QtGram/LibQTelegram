#ifndef STICKERSETSDATATABLE_H
#define STICKERSETSDATATABLE_H

#include "databasetable.h"

class StickerSetsDataTable : public DatabaseTable
{
    Q_OBJECT

    public:
        explicit StickerSetsDataTable(QObject *parent = 0);
        virtual void createSchema();
        virtual void insertQuery(QSqlQuery &queryobj, TelegramObject *telegramobject);
};

#endif // STICKERSETSDATATABLE_H
