#include "forwarddialogsmodel.h"
#include "../cache/telegramcache.h"
#include "../crypto/math.h"

ForwardDialogsModel::ForwardDialogsModel(QObject *parent) : TelegramModel(parent), _fromdialog(NULL)
{

}

Dialog *ForwardDialogsModel::fromDialog() const
{
    return this->_fromdialog;
}

void ForwardDialogsModel::setFromDialog(Dialog *dialog)
{
    if(this->_fromdialog == dialog)
        return;

    this->_fromdialog = dialog;
    this->telegramReady();
    emit fromDialogChanged();
}

QVariant ForwardDialogsModel::data(const QModelIndex &index, int role) const
{
    if(!index.isValid() || (index.row() >= this->_dialogs.count()))
        return QVariant();

    Dialog* dialog = this->_dialogs[index.row()];

    if(role == ForwardDialogsModel::ItemRole)
        return QVariant::fromValue(dialog);

    if(role == ForwardDialogsModel::TitleRole)
        return this->_telegram->dialogTitle(dialog);

    return QVariant();
}

int ForwardDialogsModel::rowCount(const QModelIndex &) const
{
    return this->_dialogs.length();
}

QHash<int, QByteArray> ForwardDialogsModel::roleNames() const
{
    QHash<int, QByteArray> roles = this->initRoles();

    roles[ForwardDialogsModel::TitleRole] = "dialogTitle";
    return roles;
}

void ForwardDialogsModel::telegramReady()
{
    if(!this->_fromdialog)
        return;

    QList<Dialog*> dialogs = TelegramCache_dialogs;

    this->beginResetModel();
    this->_dialogs.clear();

    foreach(Dialog* dialog, dialogs)
    {
        if(!this->_telegram->dialogIsWritable(dialog) || (dialog == this->_fromdialog))
            continue;

        this->_dialogs << dialog;
    }

    this->_telegram->sortDialogs(this->_dialogs);
    this->endResetModel();
}
