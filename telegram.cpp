#include "telegram.h"
#include "config/cache/telegramcache.h"
#include "config/cache/filecache.h"
#include "types/telegramhelper.h"

Telegram::Telegram(QObject *parent) : QObject(parent), _initializer(NULL)
{
}

TelegramInitializer *Telegram::initializer() const
{
    return this->_initializer;
}

User *Telegram::me() const
{
    return TelegramConfig_instance->me();
}

void Telegram::setInitializer(TelegramInitializer *initializer)
{
    if(this->_initializer == initializer)
        return;

    if(this->_initializer)
    {
        disconnect(this->_initializer, &TelegramInitializer::signUpRequested, this, 0);
        disconnect(this->_initializer, &TelegramInitializer::signInRequested, this, 0);
        disconnect(this->_initializer, &TelegramInitializer::loginCompleted, this, 0);
    }

    this->_initializer = initializer;

    connect(this->_initializer, &TelegramInitializer::signUpRequested, this, &Telegram::signUpRequested);
    connect(this->_initializer, &TelegramInitializer::signInRequested, this, &Telegram::signInRequested);
    connect(this->_initializer, &TelegramInitializer::loginCompleted, this, &Telegram::loginCompleted);

    emit initializerChanged();
}

QString Telegram::dialogTitle(Dialog *dialog)
{
    TLInt peerid = TelegramHelper::identifier(dialog->peer());

    if(TelegramHelper::isChat(dialog) || TelegramHelper::isChannel(dialog))
    {
        Chat* chat = TelegramCache_chat(peerid);
        return chat->title();
    }

    User* user = TelegramCache_user(peerid);
    return TelegramHelper::fullName(user);
}

QString Telegram::dialogDraftMessage(Dialog *dialog)
{
    if(dialog->draft())
        return dialog->draft()->message().toString();

    return QString();
}

bool Telegram::dialogHasDraftMessage(Dialog *dialog)
{
    return dialog->draft() && (dialog->draft()->constructorId() != DraftMessage::CtorDraftMessageEmpty);
}

QString Telegram::userFullName(User *user)
{
    return TelegramHelper::fullName(user);
}

QString Telegram::userStatusText(User *user)
{
    return TelegramHelper::statusText(user);
}

QString Telegram::messageText(TLInt messageid)
{
    Message* message = TelegramCache_message(messageid);
    return TelegramHelper::messageText(message);
}

FileObject *Telegram::fileObject(Dialog *dialog)
{
    return FileCache_fileObject(dialog);
}

void Telegram::signIn(const QString &phonecode)
{
    this->_initializer->signIn(phonecode);
}

void Telegram::signUp(const QString &firstname, const QString &lastname, const QString &phonecode)
{
    this->_initializer->signUp(firstname, lastname, phonecode);
}
