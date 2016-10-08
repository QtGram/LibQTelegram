#include "mtprotoupdatehandler.h"
#include "../autogenerated/telegramapi.h"
#include "../autogenerated/clientsyncmanager.h"
#include "../config/telegramconfig.h"
#include "dc/dcsessionmanager.h"

MTProtoUpdateHandler* MTProtoUpdateHandler::_instance = NULL;

MTProtoUpdateHandler::MTProtoUpdateHandler(QObject *parent) : QObject(parent), _syncmode(false)
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
    if(mtreply->constructorId() == TLTypes::Updates)
        return this->handleUpdates(mtreply);
    else if(mtreply->constructorId() == TLTypes::UpdatesState)
        this->handleUpdatesState(mtreply);

    return false;
}

void MTProtoUpdateHandler::sync()
{
    TelegramAPI::updatesGetState(DC_MAIN_SESSION);
}

bool MTProtoUpdateHandler::handleUpdates(MTProtoReply *mtreply)
{
    Updates updates;
    updates.read(mtreply);

    UpdatesState* clientstate = DC_CONFIG_UPDATES_STATE;
    ClientSyncManager::syncUpdates(&updates, clientstate);

    const TLVector<Update*>& updatelist = updates.updates();

    for(int i = 0; i < updatelist.length(); i++)
    {
        Update* update = updatelist[i];
        ClientSyncManager::syncUpdate(update, clientstate);

        if(!this->handleUpdate(update))
            return false;
    }

    return true;
}

bool MTProtoUpdateHandler::handleUpdatesState(MTProtoReply *mtreply)
{
    UpdatesState currentstate;
    currentstate.read(mtreply);

    UpdatesState* clientstate = DC_CONFIG_UPDATES_STATE;

    if(((currentstate.seq() - clientstate->seq()) > 0) && (currentstate.seq() != (clientstate->seq() + 1)))
        TelegramAPI::updatesGetDifference(DC_MAIN_SESSION, clientstate->pts(), clientstate->date(), clientstate->qts());
}

bool MTProtoUpdateHandler::handleUpdate(Update *update)
{
    if(update->constructorId() == TLTypes::UpdateNewMessage)
        return this->handleUpdateShort(update);

    return false;
}

bool MTProtoUpdateHandler::handleUpdateShort(Update *update)
{
    return true;
}

bool MTProtoUpdateHandler::handleUpdateNewAuthorization(Update *update)
{
    Q_UNUSED(update);
    return true;
}
