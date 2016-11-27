#include "telegramnotifications.h"
#include "../../mtproto/mtprotoupdatehandler.h"
#include "../../cache/telegramcache.h"

TelegramNotifications::TelegramNotifications(QObject *parent) : QObject(parent), _telegram(NULL), _currentdialog(NULL), _mute(false)
{
}

Telegram *TelegramNotifications::telegram() const
{
    return this->_telegram;
}

Dialog *TelegramNotifications::currentDialog() const
{
    return this->_currentdialog;
}

bool TelegramNotifications::mute() const
{
    return this->_mute;
}

void TelegramNotifications::setTelegram(Telegram *telegram)
{
    if(telegram == this->_telegram)
        return;

    if(this->_telegram)
        disconnect(this->_telegram, &Telegram::loginCompleted, this, 0);

    this->_telegram = telegram;
    connect(this->_telegram, &Telegram::loginCompleted, this, &TelegramNotifications::onLoginCompleted);
    emit telegramChanged();
}

void TelegramNotifications::setCurrentDialog(Dialog *dialog)
{
    if(dialog == this->_currentdialog)
        return;

    this->_currentdialog = dialog;
    emit currentDialogChanged();
}

void TelegramNotifications::setMute(bool mute)
{
    if(mute == this->_mute)
        return;

    this->_mute = mute;
    emit muteChanged();
}

void TelegramNotifications::onLoginCompleted()
{
    connect(UpdateHandler_instance, &MTProtoUpdateHandler::newSingleMessage, this, &TelegramNotifications::onIncomingMessage, Qt::UniqueConnection);
    connect(TelegramCache_instance, &TelegramCache::readHistory, this, &TelegramNotifications::onReadHistory, Qt::UniqueConnection);
}

void TelegramNotifications::onIncomingMessage(Message *message, TLLong sessionid)
{
    if(UpdateHandler_syncing || !DCSessionManager_OwnSession(sessionid))
        return;

    if(this->_mute || !this->_telegram || message->isOut() || (message->constructorId() != TLTypes::Message))
        return;

    Dialog* dialog = TelegramCache_dialog(TelegramHelper::messageToDialog(message));

    if(!dialog || (message->id() <= dialog->readInboxMaxId())) // Message is read, don't notify
        return;

    PeerNotifySettings* notifysettings = dialog->notifySettings();

    if(!message->isMentioned() && (notifysettings->isSilent() || (notifysettings->muteUntil() > 0)))
        return;

    NotificationObject notification;
    notification.setDialogId(TelegramHelper::identifier(dialog));
    notification.setDate(message->date());
    notification.setTitle(this->_telegram->dialogTitle(dialog));
    notification.setMessage(this->_telegram->messagePreview(message));
    notification.setIsCurrentDialog((dialog == this->_currentdialog));

    emit newMessage(&notification);
}

void TelegramNotifications::onReadHistory(Dialog *dialog)
{
    if((dialog->topMessage() - dialog->readInboxMaxId()) > 0)
        return;

    emit dismissNotification(dialog);
}
