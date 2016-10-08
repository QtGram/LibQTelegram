#include "mtprotoupdatehandler.h"
#include "../autogenerated/telegramapi.h"
#include "../autogenerated/clientsyncmanager.h"
#include "../config/telegramconfig.h"
#include "../types/telegramhelper.h"
#include "dc/dcsessionmanager.h"

MTProtoUpdateHandler* MTProtoUpdateHandler::_instance = NULL;

MTProtoUpdateHandler::MTProtoUpdateHandler(QObject *parent) : QObject(parent)
{
}

MTProtoUpdateHandler *MTProtoUpdateHandler::instance()
{
    if(!MTProtoUpdateHandler::_instance)
        MTProtoUpdateHandler::_instance = new MTProtoUpdateHandler();

    return MTProtoUpdateHandler::_instance;
}

bool MTProtoUpdateHandler::handle(MTProtoReply *mtreply)
{
    if(mtreply->constructorId() == TLTypes::Updates || mtreply->constructorId() == TLTypes::UpdateShortMessage)
    {
        this->handleUpdates(mtreply);
        return true;
    }

    if((mtreply->constructorId() == TLTypes::UpdatesDifference) || (mtreply->constructorId() == TLTypes::UpdatesDifferenceSlice))
    {
        this->handleUpdatesDifference(mtreply);
        return true;
    }

    if(mtreply->constructorId() == TLTypes::UpdatesState)
    {
        this->handleUpdatesState(mtreply);
        return true;
    }

    return false;
}

void MTProtoUpdateHandler::sync()
{
    TelegramAPI::updatesGetState(DC_MAIN_SESSION);
}

void MTProtoUpdateHandler::handleUpdates(MTProtoReply *mtreply)
{
    Updates updates;
    updates.read(mtreply);

    UpdatesState* clientstate = CONFIG_CLIENT_STATE;
    ClientSyncManager::syncUpdates(&updates, clientstate);

    if(updates.constructorId() == TLTypes::UpdateShortMessage)
    {
        Message* message = TelegramHelper::createMessage(&updates, TELEGRAM_CONFIG->me());
        emit newMessage(message);
    }
    else
    {
        const TLVector<Update*>& updatelist = updates.updates();
        this->handleUpdates(updatelist);
    }
}

void MTProtoUpdateHandler::handleUpdatesState(MTProtoReply *mtreply)
{
    UpdatesState serverstate;
    serverstate.read(mtreply);

    UpdatesState* clientstate = CONFIG_CLIENT_STATE;

    if(((serverstate.seq() - clientstate->seq()) > 0) && (serverstate.seq() != (clientstate->seq() + 1)))
        this->getDifferences();
}

void MTProtoUpdateHandler::handleUpdatesDifference(MTProtoReply *mtreply)
{
    UpdatesDifference updatedifference;
    updatedifference.read(mtreply);

    foreach(User* user, updatedifference.users())
        emit newUser(user);

    foreach(Chat* chat, updatedifference.chats())
        emit newChat(chat);

    foreach(Message* message, updatedifference.newMessages())
        emit newMessage(message);

    this->handleUpdates(updatedifference.otherUpdates());

    if(updatedifference.constructorId() == TLTypes::UpdatesDifferenceSlice)
    {
        this->syncState(updatedifference.intermediateState());
        this->getDifferences();
    }
}

void MTProtoUpdateHandler::syncState(UpdatesState *serverstate)
{
    UpdatesState* clientstate = CONFIG_CLIENT_STATE;

    clientstate->setPts(serverstate->pts());
    clientstate->setQts(serverstate->qts());
    clientstate->setDate(serverstate->date());
    clientstate->setSeq(serverstate->seq());
    clientstate->setUnreadCount(serverstate->unreadCount());
}

void MTProtoUpdateHandler::getDifferences()
{
    UpdatesState* clientstate = CONFIG_CLIENT_STATE;
    TelegramAPI::updatesGetDifference(DC_MAIN_SESSION, clientstate->pts(), clientstate->date(), clientstate->qts());
}

void MTProtoUpdateHandler::handleUpdates(TLVector<Update *> updatelist)
{
    UpdatesState* clientstate = CONFIG_CLIENT_STATE;

    for(int i = 0; i < updatelist.length(); i++)
    {
        Update* update = updatelist[i];
        ClientSyncManager::syncUpdate(update, clientstate);
        emit newUpdate(update);
    }
}
