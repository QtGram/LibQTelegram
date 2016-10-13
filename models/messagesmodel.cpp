#include "messagesmodel.h"
#include "../config/cache/telegramcache.h"
#include "../crypto/math.h"

#define DEFAULT_LOAD_COUNT 50

MessagesModel::MessagesModel(QObject *parent) : TelegramModel(parent), _inputpeer(NULL), _dialog(NULL)
{
    this->_loadcount = DEFAULT_LOAD_COUNT;
}

Dialog *MessagesModel::dialog() const
{
    return this->_dialog;
}

void MessagesModel::setDialog(Dialog *dialog)
{
    if(this->_dialog == dialog)
        return;

    this->_dialog = dialog;
    this->telegramReady();
    emit dialogChanged();
}

int MessagesModel::loadCount() const
{
    return this->_loadcount;
}

void MessagesModel::setLoadCount(int loadcount)
{
    if(this->_loadcount == loadcount)
        return;

    this->_loadcount = loadcount;
    emit loadCountChanged();
}

QVariant MessagesModel::data(const QModelIndex &index, int role) const
{
    if(role == MessagesModel::ItemRole)
        return QVariant::fromValue(this->_messages[index.row()]);

    return QVariant();
}

int MessagesModel::rowCount(const QModelIndex &) const
{
    return this->_messages.length();
}

QHash<int, QByteArray> MessagesModel::roleNames() const
{
    return this->initRoles();
}

void MessagesModel::loadMore()
{
    this->createInputPeer();

    int limit = this->_loadcount;

    if(this->_messages.length() < this->_loadcount)
        limit = (this->_loadcount - this->_messages.length()) + 1;

    MTProtoRequest* req = TelegramAPI::messagesGetHistory(DC_MainSession, this->_inputpeer, 0, 0, 0, limit, this->maxId(), 0);
    connect(req, &MTProtoRequest::replied, this, &MessagesModel::onMessagesGetHistoryReplied);
}

void MessagesModel::sendMessage(const QString &text)
{
    if(!this->_dialog || text.trimmed().isEmpty())
        return;

    this->createInputPeer();

    TLLong randomid = 0;
    Math::randomize(&randomid);
    TelegramAPI::messagesSendMessage(DC_MainSession, this->_inputpeer, 0, text.trimmed(), randomid, NULL, TLVector<MessageEntity*>());
}

void MessagesModel::onMessagesGetHistoryReplied(MTProtoReply *mtreply)
{
    MessagesMessages messages;
    messages.read(mtreply);

    this->beginInsertRows(QModelIndex(), 0, messages.count() - 1);

    TelegramCache_store(messages.users());
    TelegramCache_store(messages.chats());
    TelegramCache_store(messages.messages());

    const MessageCache::MessageList& newmessages = TelegramCache_messages(this->_dialog).mid(0, messages.count());

    for(int i = newmessages.count() - 1; i >= 0; i--)
        this->_messages.prepend(newmessages[i]);

    this->endInsertRows();
}

TLInt MessagesModel::maxId() const
{
    TLInt maxid = 0;

    foreach(Message* message, this->_messages)
    {
        if(message->id() >= maxid)
            maxid = message->id();
    }

    return maxid;
}

void MessagesModel::createInputPeer()
{
    if(this->_inputpeer)
        return;

    TLLong accesshash = 0;

    if(TelegramHelper::isChannel(this->_dialog) || TelegramHelper::isChat(this->_dialog))
    {
        Chat* chat = TelegramCache_chat(TelegramHelper::identifier(this->_dialog));

        if(chat)
            accesshash = chat->accessHash();
    }
    else
    {
        User* user = TelegramCache_user(TelegramHelper::identifier(this->_dialog));

        if(user)
            accesshash = user->accessHash();
    }

    this->_inputpeer = TelegramHelper::inputPeer(this->_dialog->peer(), accesshash, this);
}

void MessagesModel::telegramReady()
{
    if(!this->_dialog)
        return;

    const MessageCache::MessageList& cachemessages = TelegramCache_messages(this->_dialog);

    if(cachemessages.count() > this->_loadcount)
        this->_messages = cachemessages.mid(cachemessages.count() - this->_loadcount);
    else
        this->_messages << cachemessages;

    if(this->_messages.length() < this->_loadcount)
    {
        this->loadMore();
        return;
    }

    this->resetInternalData();
}
