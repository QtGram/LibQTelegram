#include "stickersetsmodel.h"
#include "../cache/sticker/stickercache.h"

StickerSetsModel::StickerSetsModel(QObject *parent) : TelegramModel(parent)
{

}

QVariant StickerSetsModel::data(const QModelIndex &index, int role) const
{
    if(!index.isValid() || (index.row() >= this->_stickersets.count()))
        return QVariant();

    StickerSet* stickerset = this->_stickersets[index.row()];

    if(role == StickerSetsModel::ItemRole)
        return QVariant::fromValue(stickerset);

    if(role == StickerSetsModel::TitleRole)
        return stickerset->title().toString();

    if(role == StickerSetsModel::ShortNameRole)
        return stickerset->shortName().toString();

    if(role == StickerSetsModel::StickerPreviewRole)
        return QVariant::fromValue(StickerCache_stickerPreview(stickerset));

    return QVariant();
}

int StickerSetsModel::rowCount(const QModelIndex &) const
{
    return this->_stickersets.length();
}

QHash<int, QByteArray> StickerSetsModel::roleNames() const
{
    QHash<int, QByteArray> roles = this->initRoles();

    roles[StickerSetsModel::TitleRole] = "title";
    roles[StickerSetsModel::ShortNameRole] = "shortName";
    roles[StickerSetsModel::StickerPreviewRole] = "stickerPreview";

    return roles;
}

void StickerSetsModel::telegramReady()
{
    this->beginResetModel();
    StickerCache_populateInstalled(this->_stickersets);
    this->endResetModel();
}
