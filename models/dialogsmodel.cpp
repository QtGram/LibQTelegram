#include "dialogsmodel.h"
#include "../config/telegramruntime.h"

DialogsModel::DialogsModel(QObject *parent) : TelegramModel(parent)
{
}

QVariant DialogsModel::data(const QModelIndex &index, int role) const
{
    if(role == DialogsModel::ItemRole)
        return QVariant::fromValue(this->_dialogs[index.row()]);

    return QVariant();
}

int DialogsModel::rowCount(const QModelIndex &) const
{
    return this->_dialogs.length();
}

QHash<int, QByteArray> DialogsModel::roleNames() const
{
    return this->initRoles();
}

void DialogsModel::telegramReady()
{
    this->beginResetModel();
    this->_dialogs = TELEGRAM_RUNTIME->dialogList();
    this->endResetModel();
}
