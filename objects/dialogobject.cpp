#include "dialogobject.h"
#include "../types/telegramhelper.h"

DialogObject::DialogObject(QObject *parent) : QObject(parent)
{
    this->_user = NULL;
    this->_chat = NULL;
    this->_message = NULL;
}

User *DialogObject::user() const
{
    return this->_user;
}

Chat *DialogObject::chat() const
{
    return this->_chat;
}

QString DialogObject::title() const
{
    if(this->_chat)
        return this->_chat->title();

    return TelegramHelper::fullName(this->_user);
}

MessageObject *DialogObject::topMessage() const
{
    return this->_message;
}

void DialogObject::setUser(User* user)
{
    if(this->_user == user)
        return;

    this->_user = user;
    emit userChanged();
}

void DialogObject::setChat(Chat* chat)
{
    if(this->_chat == chat)
        return;

    this->_chat = chat;
    emit userChanged();
}

void DialogObject::setTopMessage(MessageObject *message)
{
    if(this->_message == message)
        return;

    this->_message = message;
    emit userChanged();
}
