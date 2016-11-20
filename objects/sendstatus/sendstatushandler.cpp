#include "sendstatushandler.h"
#include "../../mtproto/mtprotoupdatehandler.h"
#include "../../cache/telegramcache.h"

SendStatusHandler* SendStatusHandler::_instance = NULL;

SendStatusHandler::SendStatusHandler(QObject *parent) : QObject(parent)
{
    connect(UpdateHandler_instance, SIGNAL(typing(Update*)), this, SLOT(onTyping(Update*)));
}

SendStatusHandler *SendStatusHandler::instance()
{
    if(!SendStatusHandler::_instance)
        SendStatusHandler::_instance = new SendStatusHandler();

    return SendStatusHandler::_instance;
}

bool SendStatusHandler::hasSendStatus(Dialog *dialog) const
{
    return this->_dialogstatus.contains(dialog);
}

QString SendStatusHandler::dialogSendStatus(Dialog *dialog) const
{
    if(!this->_dialogstatus.contains(dialog))
        return QString();

    return this->_dialogstatus[dialog]->sendStatusText();
}

void SendStatusHandler::onTyping(Update *update)
{
    Q_ASSERT((update->constructorId() == TLTypes::UpdateUserTyping) ||
             (update->constructorId() == TLTypes::UpdateChatUserTyping));

    TLInt dialogid = (update->constructorId() == TLTypes::UpdateChatUserTyping) ? update->chatId() : update->userId();
    Dialog* dialog = TelegramCache_dialog(dialogid);

    if(!dialog)
        return;

    SendStatusObject* sendstatus = NULL;

    if(!this->_dialogstatus.contains(dialog))
    {
        sendstatus = new SendStatusObject(dialog, this);
        connect(sendstatus, &SendStatusObject::expired, this, &SendStatusHandler::onSendStatusExpired);

        this->_dialogstatus[dialog] = sendstatus;
    }
    else
        sendstatus = this->_dialogstatus[dialog];

    User* user = TelegramCache_user(update->userId());

    if(user)
        sendstatus->update(user, update->action(), (update->constructorId() == TLTypes::UpdateChatUserTyping));

    emit sendStatusUpdated(dialog);
}

void SendStatusHandler::onSendStatusExpired()
{
    SendStatusObject* sendstatus = qobject_cast<SendStatusObject*>(this->sender());
    Dialog* dialog = sendstatus->dialog();

    this->_dialogstatus.remove(sendstatus->dialog());
    sendstatus->deleteLater();

    emit sendStatusUpdated(dialog);

}
