#include "sendstatusobject.h"
#include "../../types/telegramhelper.h"

/*
 * From Telegram documentation: https://core.telegram.org/constructor/updateUserTyping
 * If no repeated update received after 6 seconds, it should be considered that the user stopped doing whatever he's been doing.
 */

#define StatusTimeout 6000 // 6 seconds

SendStatusObject::SendStatusObject(Dialog* dialog, QObject *parent) : QObject(parent), _dialog(dialog)
{
    this->_timexpired = new QTimer(this);
    this->_timexpired->setSingleShot(true);
    this->_timexpired->setInterval(StatusTimeout);
    this->_timexpired->start();

    connect(this->_timexpired, &QTimer::timeout, this, &SendStatusObject::expired);
}

Dialog *SendStatusObject::dialog() const
{
    return this->_dialog;
}

void SendStatusObject::update(User *user, SendMessageAction* sendmessageaction, bool fromchat)
{
    if(this->isCancelled(sendmessageaction))
        this->_users.remove(user);
    else if(sendmessageaction->constructorId() == TLTypes::SendMessageTypingAction)
        this->_users << user;

    if(this->_users.isEmpty())
        return; // Let the timer to expire

    this->updateStatusText(sendmessageaction, fromchat);
    this->_timexpired->start();
}

QString SendStatusObject::sendStatusText() const
{
    return this->_sendstatustext;
}

bool SendStatusObject::isCancelled(SendMessageAction *sendaction) const
{
    return (sendaction->constructorId() == TLTypes::SendMessageCancelAction) || (sendaction->constructorId() == TLTypes::SendMessageGameStopAction);
}

void SendStatusObject::updateStatusText(SendMessageAction* sendmessageaction, bool fromchat)
{
    if(!sendmessageaction)
    {
        this->_sendstatustext.clear();
        return;
    }

    if(fromchat)
    {
        QString prefix = this->createUserPrefix();
        this->_sendstatustext = prefix.arg(this->createStatusText(sendmessageaction));
        return;
    }

    this->_sendstatustext = this->createStatusText(sendmessageaction);
    this->_sendstatustext = this->_sendstatustext.mid(0, 1).toUpper() + this->_sendstatustext.mid(1); // Capitalize
}

QString SendStatusObject::createStatusText(SendMessageAction *sendmessageaction) const
{
    switch(sendmessageaction->constructorId())
    {
        case TLTypes::SendMessageTypingAction:
            return tr("typing...");

        case TLTypes::SendMessageRecordVideoAction:
            return tr("recording video...");

        case TLTypes::SendMessageUploadVideoAction:
        {
            if(sendmessageaction->progress() <= 0)
                return tr("uploading video...");

            return tr("uploading video (%1%%)...").arg(sendmessageaction->progress());
        }

        case TLTypes::SendMessageRecordAudioAction:
            return tr("recording audio...");

        case TLTypes::SendMessageUploadAudioAction:
        {
            if(sendmessageaction->progress() <= 0)
                return tr("uploading video...");

            return tr("uploading audio (%1%%)...").arg(sendmessageaction->progress());
        }

        case TLTypes::SendMessageUploadPhotoAction:
        {
            if(sendmessageaction->progress() <= 0)
                return tr("uploading photo...");

            return tr("uploading photo (%1%%)...").arg(sendmessageaction->progress());
        }

        case TLTypes::SendMessageUploadDocumentAction:
        {
            if(sendmessageaction->progress() <= 0)
                return tr("uploading file...");

            return tr("uploading file (%1%%)...").arg(sendmessageaction->progress());
        }

        case TLTypes::SendMessageGeoLocationAction:
            return tr("sending location...");

        case TLTypes::SendMessageChooseContactAction:
            return tr("choosing contact...");

        case TLTypes::SendMessageGamePlayAction:
            return tr("playing game");

        default:
            break;
    }

    return QString();
}

QString SendStatusObject::createUserPrefix() const
{
    if(this->_users.count() == 1)
    {
        QList<User*> users = this->_users.toList();
        return tr("%1 is %2").arg(users.first()->firstName().toString());
    }
    else if(this->_users.count() == 2)
    {
        QList<User*> users = this->_users.toList();
        return tr("%1 and %2 are %3").arg(users.first()->firstName().toString(), users.last()->firstName().toString());
    }

    return tr("%1 users are %2").arg(this->_users.count());
}
