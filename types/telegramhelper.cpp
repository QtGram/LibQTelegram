#include "telegramhelper.h"

TelegramHelper::TelegramHelper()
{

}

QString TelegramHelper::fullName(User *user)
{
    if(user->lastName().isEmpty())
        return user->firstName();

    return user->firstName() + " " + user->lastName();
}

bool TelegramHelper::isChat(Dialog *dialog)
{
    return dialog->peer()->constructorId() == TLTypes::PeerChat;
}

bool TelegramHelper::isChannel(Dialog *dialog)
{
    return dialog->peer()->constructorId() == TLTypes::PeerChannel;
}

TLInt TelegramHelper::identifier(User *user)
{
    return user->id();
}

TLInt TelegramHelper::identifier(Chat *chat)
{
    return chat->id();
}

TLInt TelegramHelper::identifier(Dialog *dialog)
{
    if(dialog->peer()->constructorId() == TLTypes::PeerChannel)
        return dialog->peer()->channelId();

    if(dialog->peer()->constructorId() == TLTypes::PeerChat)
        return dialog->peer()->chatId();

    if(dialog->peer()->constructorId() == TLTypes::PeerUser)
        return dialog->peer()->userId();

    Q_ASSERT(false);
    return 0;
}

TLInt TelegramHelper::identifier(Message *message)
{
    return message->id();
}
