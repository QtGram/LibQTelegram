#include "cacheinitializer.h"
#include "../../autogenerated/telegramapi.h"
#include "../../mtproto/dc/dcsessionmanager.h"
#include "telegramcache.h"

CacheInitializer::CacheInitializer(QObject *parent) : QObject(parent), _state(CacheInitializer::First)
{

}

void CacheInitializer::initialize()
{
    if(this->_state == CacheInitializer::RequestContacts)
        this->requestContacts();
    else if(this->_state == CacheInitializer::RequestDialogs)
        this->requestDialogs();

    if(this->_state > CacheInitializer::Last)
    {
        TelegramCache_save;
        emit initialized();
    }
}

void CacheInitializer::requestContacts()
{
    MTProtoRequest* req = TelegramAPI::contactsGetContacts(DC_MAIN_SESSION, TLString());
    connect(req, &MTProtoRequest::replied, this, &CacheInitializer::onRequestContactsReplied);
}

void CacheInitializer::requestDialogs()
{
    MTProtoRequest* req = TelegramAPI::messagesGetDialogs(DC_MAIN_SESSION, 0, 0, NULL, 0);
    connect(req, &MTProtoRequest::replied, this, &CacheInitializer::onRequestDialogsReplied);
}

void CacheInitializer::onRequestContactsReplied(MTProtoReply *mtreply)
{
    ContactsContacts contacts;
    contacts.read(mtreply);

    TelegramCache_store(contacts.users());

    this->_state++;
    this->initialize();
}

void CacheInitializer::onRequestDialogsReplied(MTProtoReply *mtreply)
{
    MessagesDialogs messagesdialogs;
    messagesdialogs.read(mtreply);

    TelegramCache_store(messagesdialogs.dialogs());
    TelegramCache_store(messagesdialogs.users());
    TelegramCache_store(messagesdialogs.chats());
    TelegramCache_store(messagesdialogs.messages());

    this->_state++;
    this->initialize();
}
