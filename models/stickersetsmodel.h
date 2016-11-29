#ifndef STICKERSETSMODEL_H
#define STICKERSETSMODEL_H

#include "abstract/telegrammodel.h"

class StickerSetsModel : public TelegramModel
{
    Q_OBJECT

    public:
        enum StickerSetRoles {
            TitleRole = Qt::UserRole + 10,
            ShortNameRole,
            StickerPreviewRole,
        };

    public:
        explicit StickerSetsModel(QObject *parent = 0);

    public:
        virtual QVariant data(const QModelIndex &index, int role) const;
        virtual int rowCount(const QModelIndex& = QModelIndex()) const;
        virtual QHash<int, QByteArray> roleNames() const;

    protected:
        virtual void telegramReady();

    private:
        QList<StickerSet*> _stickersets;
};

#endif // STICKERSETSMODEL_H
