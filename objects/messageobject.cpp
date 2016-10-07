#include "messageobject.h"

MessageObject::MessageObject(QObject *parent) : QObject(parent), _fromuser(NULL), _message(NULL)
{

}

Message *MessageObject::message() const
{
    return this->_message;
}

User *MessageObject::fromUser() const
{
    return this->_fromuser;
}

QString MessageObject::messageText() const
{
    if(!this->_message)
        return QString();

    return this->_message->message().toString();
}

void MessageObject::setMessage(Message *message)
{
    if(this->_message == message)
        return;

    this->_message = message;
    emit messageChanged();
    emit messageTextChanged();
}

void MessageObject::setFromUser(User *fromuser)
{
    if(this->_fromuser == fromuser)
        return;

    this->_fromuser = fromuser;
    emit fromUserChanged();
}
