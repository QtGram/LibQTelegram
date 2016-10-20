#include "messagesmodel.h"
#include "../config/cache/telegramcache.h"
#include "../crypto/math.h"

#define DEFAULT_LOAD_COUNT 50

MessagesModel::MessagesModel(QObject *parent) : TelegramModel(parent), _inputpeer(NULL), _dialog(NULL), _athistoryend(false)
{
    this->_loadcount = DEFAULT_LOAD_COUNT;

    connect(TelegramCache_instance, &TelegramCache::newMessage, this, &MessagesModel::onNewMessage);
    connect(TelegramCache_instance, &TelegramCache::deleteMessage, this, &MessagesModel::onDeleteMessage);

    connect(this, &MessagesModel::dialogChanged, this, &MessagesModel::titleChanged);
    connect(this, &MessagesModel::dialogChanged, this, &MessagesModel::statusTextChanged);
    connect(this, &MessagesModel::dialogChanged, this, &MessagesModel::isChatChanged);
    connect(this, &MessagesModel::dialogChanged, this, &MessagesModel::isBroadcastChanged);
    connect(this, &MessagesModel::dialogChanged, this, &MessagesModel::isMegaGroupChanged);
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

QString MessagesModel::title() const
{
    if(!this->_telegram)
        return QString();

    return this->_telegram->dialogTitle(this->_dialog);
}

QString MessagesModel::statusText() const
{
    if(!this->_telegram)
        return QString();

    return this->_telegram->dialogStatusText(this->_dialog);
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

bool MessagesModel::isChat() const
{
    if(!this->_dialog)
        return false;

    return TelegramHelper::isChat(this->_dialog);
}

bool MessagesModel::isBroadcast() const
{
    if(!this->_dialog || !TelegramHelper::isChannel(this->_dialog))
        return false;

    Chat* chat = TelegramCache_chat(TelegramHelper::identifier(this->_dialog));

    if(!chat)
        return false;

    return chat->isBroadcast();
}

bool MessagesModel::isMegaGroup() const
{
    if(!this->_dialog || !TelegramHelper::isChannel(this->_dialog))
        return false;

    Chat* chat = TelegramCache_chat(TelegramHelper::identifier(this->_dialog));

    if(!chat)
        return false;

    return chat->isMegagroup();
}

QVariant MessagesModel::data(const QModelIndex &index, int role) const
{
    if(!index.isValid() || (index.row() >= this->_messages.length()))
        return QVariant();

    Message* message = this->_messages[index.row()];

    if(role == MessagesModel::ItemRole)
        return QVariant::fromValue(message);

    if(role == MessagesModel::MessageFrom)
        return this->messageFrom(message);

    if(role == MessagesModel::MessageText)
        return this->_telegram->messageText(message);

    if(role == MessagesModel::IsServiceMessageRole)
        return (message->constructorId() == TLTypes::MessageService);

    return QVariant();
}

int MessagesModel::rowCount(const QModelIndex &) const
{
    return this->_messages.length();
}

QHash<int, QByteArray> MessagesModel::roleNames() const
{
    QHash<int, QByteArray> roles = this->initRoles();

    roles[MessagesModel::MessageFrom] = "messageFrom";
    roles[MessagesModel::MessageText] = "messageText";
    roles[MessagesModel::IsServiceMessageRole] = "isServiceMessage";

    return roles;
}

void MessagesModel::loadMore()
{
    if(this->_athistoryend)
        return;

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

    this->_pendingmessages << TelegramHelper::createMessage(text, TelegramConfig_me, this->_dialog->peer());
    MTProtoRequest* req = TelegramAPI::messagesSendMessage(DC_MainSession, this->_inputpeer, 0, text.trimmed(), randomid, NULL, TLVector<MessageEntity*>());
    connect(req, &MTProtoRequest::replied, this, &MessagesModel::onMessagesSendMessageReplied);
}

void MessagesModel::onMessagesGetHistoryReplied(MTProtoReply *mtreply)
{
    MessagesMessages messages;
    messages.read(mtreply);

    if(messages.constructorId() == TLTypes::MessagesMessages)
        this->_athistoryend = true;
    else
        this->_athistoryend = (this->_messages.count() >= messages.count());

    int count = messages.messages().count();

    this->beginInsertRows(QModelIndex(), 0, count - 1);

    TelegramCache_store(messages.users());
    TelegramCache_store(messages.chats());
    TelegramCache_store(messages.messages());

    QList<Message*> newmessages = TelegramCache_messages(this->_dialog, count);

    for(int i = newmessages.count() - 1; i >= 0; i--)
        this->_messages.prepend(newmessages[i]);

    this->endInsertRows();
}

void MessagesModel::onMessagesSendMessageReplied(MTProtoReply *mtreply)
{
    if(this->_pendingmessages.isEmpty())
        return;

    Updates updates;
    updates.read(mtreply);

    Q_ASSERT(updates.constructorId() == TLTypes::UpdateShortSentMessage);

    Message* message = this->_pendingmessages.takeFirst();
    message->setId(updates.id());
    message->setFlags(updates.flags());
    message->setMedia(updates.media());

    TelegramCache_cacheNotify(message);
}

void MessagesModel::onNewMessage(Message *message)
{
    if(!this->_dialog || (TelegramHelper::identifier(this->_dialog) != TelegramHelper::messageToDialog(message)))
        return;

    this->beginInsertRows(QModelIndex(), 0, 0);
    this->_messages.prepend(message);
    this->endInsertRows();
}

void MessagesModel::onDeleteMessage(Message* message)
{
    if(!this->_dialog || (TelegramHelper::identifier(this->_dialog) != TelegramHelper::messageToDialog(message)))
        return;

    for(int i = 0; i < this->_messages.length(); i++)
    {
        if(this->_messages[i]->id() != message->id())
            continue;

        this->beginRemoveRows(QModelIndex(), i, i);
        this->_messages.removeAt(i);
        this->endRemoveRows();
        break;
    }
}

QString MessagesModel::messageFrom(Message *message) const
{
    TelegramObject* tgobj = this->_telegram->messageFrom(message);

    if(!tgobj)
        return QString();

    switch(tgobj->constructorId())
    {
        case TLTypes::Chat:
        case TLTypes::ChatForbidden:
        case TLTypes::Channel:
        case TLTypes::ChannelForbidden:
            return qobject_cast<Chat*>(tgobj)->title();

        case TLTypes::User:
            return TelegramHelper::fullName(qobject_cast<User*>(tgobj));

        default:
            break;
    }

    return QString();
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

    this->_messages = TelegramCache_messages(this->_dialog, this->_loadcount);

    if(this->_messages.length() < this->_loadcount)
    {
        this->loadMore();
        return;
    }

    this->resetInternalData();
}
