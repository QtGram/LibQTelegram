#include "dialogsmodel.h"
#include "../config/telegramruntime.h"

DialogsModel::DialogsModel(QObject *parent) : TelegramModel(parent)
{
    connect(TELEGRAM_RUNTIME, &TelegramRuntime::dialogsChanged, this, &DialogsModel::sortDialogs);
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

    std::sort(this->_dialogs.begin(), this->_dialogs.end(), [](DialogObject* dlg1, DialogObject* dlg2) {
        return dlg1->topMessage()->message()->date() > dlg2->topMessage()->message()->date();
    });

    this->endResetModel();
}

void DialogsModel::telegramReady()
{
    this->_dialogs = TELEGRAM_RUNTIME->dialogList();
    this->sortDialogs();
}
