#include "telegrammodel.h"
#include "../config/telegramconfig.h"

TelegramModel::TelegramModel(QObject *parent) : QAbstractListModel(parent), _telegram(NULL)
{

}

Telegram *TelegramModel::telegram() const
{
    return this->_telegram;
}

void TelegramModel::setTelegram(Telegram *telegram)
{
    if(this->_telegram == telegram)
        return;

    if(this->_telegram)
        disconnect(this->_telegram, &Telegram::loginCompleted, this, 0);

    this->_telegram = telegram;
    connect(this->_telegram, &Telegram::loginCompleted, this, [this]() { this->telegramReady(); });

    if(IS_LOGGED_IN)
        this->telegramReady();

    emit telegramChanged();
}

QHash<int, QByteArray> TelegramModel::initRoles() const
{
    QHash<int, QByteArray> roles;

    roles[TelegramModel::ItemRole] = "item";
    return roles;
}
