#include "dialogsmodel.h"
#include "../config/cache/telegramcache.h"

DialogsModel::DialogsModel(QObject *parent) : TelegramModel(parent)
{
    connect(TelegramCache_instance, &TelegramCache::dialogsChanged, this, &DialogsModel::sortDialogs);
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

void DialogsModel::sortDialogs()
{
    this->beginResetModel();

    std::sort(this->_dialogs.begin(), this->_dialogs.end(), [](Dialog* dlg1, Dialog* dlg2) {
        Message* msg1 = TelegramCache_message(dlg1->topMessage());
        Message* msg2 = TelegramCache_message(dlg2->topMessage());

        if(!msg1 || !msg2)
            return false;

        return msg1->date() > msg2->date();
    });

    this->endResetModel();
}

void DialogsModel::telegramReady()
{
    this->_dialogs = TelegramCache_dialogs;
    this->sortDialogs();
}
