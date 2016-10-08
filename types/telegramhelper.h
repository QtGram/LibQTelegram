#ifndef TELEGRAMHELPER_H
#define TELEGRAMHELPER_H

#include "basic.h"
#include "../autogenerated/types/types.h"

class TelegramHelper
{
    private:
        TelegramHelper();

    public:
        static Message* createMessage(Updates *updates, User *me);
        static QString fullName(User* user);
        static bool isChat(Dialog* dialog);
        static bool isChannel(Dialog* dialog);
        static TLInt identifier(User* user);
        static TLInt identifier(Chat* chat);
        static TLInt identifier(Peer* peer);
        static TLInt identifier(Dialog* dialog);
        static TLInt identifier(Message* message);
        static TLInt dialogIdentifier(Message* message);
};

#endif // TELEGRAMHELPER_H
