#include "telegramsortfilterproxymodel.h"

TelegramSortFilterProxyModel::TelegramSortFilterProxyModel(QObject *parent) : QSortFilterProxyModel(parent), _telegram(NULL)
{

}

Telegram *TelegramSortFilterProxyModel::telegram() const
{
    return this->_telegram;
}

void TelegramSortFilterProxyModel::setTelegram(Telegram *telegram)
{
    if(this->_telegram == telegram)
        return;

    this->_telegram = telegram;
    this->updateTelegram(telegram);
    emit telegramChanged();
}
