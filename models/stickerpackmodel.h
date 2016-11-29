#ifndef STICKERPACKMODEL_H
#define STICKERPACKMODEL_H

#include "abstract/telegrammodel.h"

class StickerPackModel : public TelegramModel
{
    Q_OBJECT

    Q_PROPERTY(StickerSet* stickerSet READ stickerSet WRITE setStickerSet NOTIFY stickerSetChanged)
    Q_PROPERTY(QString title READ title NOTIFY titleChanged)

    public:
        explicit StickerPackModel(QObject *parent = 0);
        StickerSet* stickerSet() const;
        void setStickerSet(StickerSet* stickerset);
        QString title() const;

    public:
        virtual QVariant data(const QModelIndex &index, int role) const;
        virtual int rowCount(const QModelIndex& = QModelIndex()) const;
        virtual QHash<int, QByteArray> roleNames() const;

    protected:
        virtual void telegramReady();

    signals:
        void stickerSetChanged();
        void titleChanged();

    private:
        StickerSet* _stickerset;
        QList<Document*> _stickers;
};

#endif // STICKERPACKMODEL_H
