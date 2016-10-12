#include "messagesmodel.h"
#include "../config/cache/telegramcache.h"

MessagesModel::MessagesModel(QObject *parent) : TelegramModel(parent)
{

}

QVariant MessagesModel::data(const QModelIndex &index, int role) const
{
    if(role == MessagesModel::ItemRole)
        return QVariant::fromValue(this->_messages[index.row()]);

    return QVariant();
}

int MessagesModel::rowCount(const QModelIndex &) const
{
    return this->_messages.length();
}

QHash<int, QByteArray> MessagesModel::roleNames() const
{
    return this->initRoles();
}

void MessagesModel::sortMessages()
{
    this->beginResetModel();

    std::sort(this->_messages.begin(), this->_messages.end(), [](Message* msg1, Message* msg2) {
        return msg1->date() > msg2->date();
    });

    this->endResetModel();
}

void MessagesModel::telegramReady()
{
    this->_messages = TelegramCache_messages.values();
    this->sortMessages();
}
