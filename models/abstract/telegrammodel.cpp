#include "telegrammodel.h"
#include "../../config/telegramconfig.h"
#include "../../cache/telegramcache.h"

TelegramModel::TelegramModel(QObject *parent) : QAbstractListModel(parent), _telegram(NULL), _initializing(false), _loading(false)
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
        disconnect(this->_telegram->initializer(), &TelegramInitializer::loginCompleted, this, 0);

    this->_telegram = telegram;
    connect(this->_telegram->initializer(), &TelegramInitializer::loginCompleted, this, [this]() { this->telegramReady(); });

    if(DCConfig_isLoggedIn)
        this->telegramReady();

    emit telegramChanged();
}

bool TelegramModel::initializing() const
{
    return this->_initializing;
}

bool TelegramModel::loading() const
{
    return this->_loading;
}

void TelegramModel::setInitializing(bool initializing)
{
    if(this->_initializing == initializing)
        return;

    this->_initializing = initializing;
    emit initializingChanged();
}

void TelegramModel::setLoading(bool loading)
{
    if(this->_loading == loading)
        return;

    this->_loading = loading;
    emit loadingChanged();
}

QHash<int, QByteArray> TelegramModel::initRoles() const
{
    QHash<int, QByteArray> roles;

    roles[TelegramModel::ItemRole] = "item";
    return roles;
}
