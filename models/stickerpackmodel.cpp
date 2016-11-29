#include "stickerpackmodel.h"
#include "../cache/sticker/stickercache.h"

StickerPackModel::StickerPackModel(QObject *parent) : TelegramModel(parent), _stickerset(NULL)
{
    connect(this, &StickerPackModel::stickerSetChanged, this, &StickerPackModel::titleChanged);
}

StickerSet *StickerPackModel::stickerSet() const
{
    return this->_stickerset;
}

void StickerPackModel::setStickerSet(StickerSet *stickerset)
{
    if(this->_stickerset == stickerset)
        return;

    this->_stickerset = stickerset;
    this->telegramReady();
    emit stickerSetChanged();
}

QString StickerPackModel::title() const
{
    if(!this->_stickerset)
        return QString();

    return this->_stickerset->title().toString();
}

QVariant StickerPackModel::data(const QModelIndex &index, int role) const
{
    if(!index.isValid() || (index.row() >= this->_stickers.count()))
        return QVariant();

    Document* sticker = this->_stickers[index.row()];

    if(role == StickerPackModel::ItemRole)
        return QVariant::fromValue(sticker);

    return QVariant();
}

int StickerPackModel::rowCount(const QModelIndex &) const
{
    return this->_stickers.length();
}

QHash<int, QByteArray> StickerPackModel::roleNames() const
{
    return this->initRoles();
}

void StickerPackModel::telegramReady()
{
    if(!this->_telegram || !this->_stickerset)
        return;

    this->beginResetModel();
    StickerCache_populatePack(this->_stickerset, this->_stickers);
    this->endResetModel();
}
