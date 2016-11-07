#include "messagesmodel.h"
#include "../config/cache/telegramcache.h"
#include "../objects/sendstatus/sendstatushandler.h"
#include "../crypto/math.h"

#define MessagesFirstLoad 30
#define MessagesPerPage   50

MessagesModel::MessagesModel(QObject *parent) : TelegramModel(parent), _inputpeer(NULL), _dialog(NULL), _timaction(NULL), _newmessageindex(-1), _newmessageid(0), _fetchmore(true), _atstart(false), _loadcount(MessagesFirstLoad)
{
    connect(this, &MessagesModel::dialogChanged, this, &MessagesModel::titleChanged);
    connect(this, &MessagesModel::dialogChanged, this, &MessagesModel::statusTextChanged);
    connect(this, &MessagesModel::dialogChanged, this, &MessagesModel::isChatChanged);
    connect(this, &MessagesModel::dialogChanged, this, &MessagesModel::isBroadcastChanged);
    connect(this, &MessagesModel::dialogChanged, this, &MessagesModel::isMegaGroupChanged);
    connect(this, &MessagesModel::dialogChanged, this, &MessagesModel::isWritableChanged);
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

    if(SendStatusHandler_hasSendStatus(this->_dialog))
        return SendStatusHandler_dialogSendStatus(this->_dialog);

    return this->_telegram->dialogStatusText(this->_dialog);
}

int MessagesModel::newMessageIndex() const
{
    return this->_newmessageindex;
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

bool MessagesModel::isWritable() const
{
    if(!this->_dialog)
        return false;

    TLInt dialogid = TelegramHelper::identifier(this->_dialog);

    if(this->isBroadcast())
        return dialogid == TelegramConfig_me->id();

    if(TelegramHelper::isChat(this->_dialog) || TelegramHelper::isChannel(this->_dialog))
    {
        Chat* chat = TelegramCache_chat(dialogid);

        if(!chat)
            return false;

        return (chat->constructorId() == TLTypes::Chat) || (chat->constructorId() == TLTypes::Channel);
    }

    return true;
}

bool MessagesModel::canFetchMore(const QModelIndex &) const
{
    return this->_fetchmore && !this->_loading;
}

void MessagesModel::fetchMore(const QModelIndex &)
{
    if(this->_loading)
        return;

    this->setLoading(true);

    this->_loadcount = MessagesPerPage;
    int count = this->loadHistoryFromCache();

    if(count >= this->_loadcount) // We have enough cached messages
    {
        this->setLoading(false);
        return;
    }

    this->createInputPeer();

    MTProtoRequest* req = TelegramAPI::messagesGetHistory(DC_MainSession, this->_inputpeer, 0, 0, this->_messages.count(), this->_loadcount, 0, 0);
    connect(req, &MTProtoRequest::replied, this, &MessagesModel::onMessagesGetHistoryReplied);
}

QVariant MessagesModel::data(const QModelIndex &index, int role) const
{
    if(!index.isValid() || (index.row() >= this->_messages.length()))
        return QVariant();

    Message* message = this->_messages[index.row()];

    if(role == MessagesModel::ItemRole)
        return QVariant::fromValue(message);

    if(role == MessagesModel::MessageFromRole)
        return this->messageFrom(message);

    if(role == MessagesModel::MessageTextRole)
        return this->_telegram->messageText(message);

    if(role == MessagesModel::ReplyItemRole)
        return message->replyToMsgId() ? QVariant::fromValue(TelegramCache_message(message->replyToMsgId(), this->_dialog)) : QVariant();

    if(role == MessagesModel::ReplyFromRole)
        return this->messageFrom(TelegramCache_message(message->replyToMsgId(), this->_dialog));

    if(role == MessagesModel::ReplyTextRole)
        return this->_telegram->messagePreview(TelegramCache_message(message->replyToMsgId(), this->_dialog));

    if(role == MessagesModel::IsMessageNewRole)
        return message->id() == this->_newmessageid;

    if(role == MessagesModel::IsMessageOutRole)
        return message->isOut();

    if(role == MessagesModel::IsMessageServiceRole)
        return (message->constructorId() == TLTypes::MessageService);

    if(role == MessagesModel::IsMessageMediaRole)
        return message->media() && (message->media()->constructorId() != TLTypes::MessageMediaEmpty);

    if(role == MessagesModel::MessageDateRole)
        return TelegramHelper::dateString(message->date());

    if(role == MessagesModel::MessageHasReplyRole)
        return message->replyToMsgId() != 0;

    if(role == MessagesModel::IsMessageForwardedRole)
        return message->fwdFrom() != NULL;

    if(role == MessagesModel::IsMessageUnreadRole)
        return this->_dialog->readOutboxMaxId() < message->id();

    if(role == MessagesModel::IsMessageEditedRole)
        return message->editDate() != 0;

    if(role == MessagesModel::NeedsPeerImageRole)
    {
        if(message->constructorId() == TLTypes::MessageService)
            return false;

        if(message != this->_messages.last())
        {
            Message* previousmessage = this->_messages[index.row() + 1];
            return message->fromId() != previousmessage->fromId();
        }

        return true;
    }

    if(role == MessagesModel::ForwardedFromUserRole)
        if (message->fwdFrom())
            return QVariant::fromValue(TelegramCache_user(message->fwdFrom()->fromId()));

    if(role == MessagesModel::ForwardedFromNameRole)
    {
        User* user = TelegramCache_user(message->fwdFrom()->fromId());
        if (user) {
            return TelegramHelper::fullName(user);
        }
    }

    return QVariant();
}

int MessagesModel::rowCount(const QModelIndex &) const
{
    return this->_messages.length();
}

QHash<int, QByteArray> MessagesModel::roleNames() const
{
    QHash<int, QByteArray> roles = this->initRoles();

    roles[MessagesModel::MessageFromRole] = "messageFrom";
    roles[MessagesModel::MessageTextRole] = "messageText";
    roles[MessagesModel::MessageDateRole] = "messageDate";
    roles[MessagesModel::MessageHasReplyRole] = "messageHasReply";
    roles[MessagesModel::ReplyItemRole] = "replyItem";
    roles[MessagesModel::ReplyFromRole] = "replyFrom";
    roles[MessagesModel::ReplyTextRole] = "replyText";
    roles[MessagesModel::IsMessageForwardedRole] = "isMessageForwarded";
    roles[MessagesModel::IsMessageNewRole] = "isMessageNew";
    roles[MessagesModel::IsMessageOutRole] = "isMessageOut";
    roles[MessagesModel::IsMessageServiceRole] = "isMessageService";
    roles[MessagesModel::IsMessageMediaRole] = "isMessageMedia";
    roles[MessagesModel::IsMessageUnreadRole] = "isMessageUnread";
    roles[MessagesModel::IsMessageEditedRole] = "isMessageEdited";
    roles[MessagesModel::NeedsPeerImageRole] = "needsPeerImage";
    roles[MessagesModel::ForwardedFromUserRole] = "forwardedFromUser";
    roles[MessagesModel::ForwardedFromNameRole] = "forwardedFromName";

    return roles;
}

int MessagesModel::loadHistoryFromCache()
{
    QList<Message*> newmessages = TelegramCache_messages(this->_dialog, this->_messages.count(), this->_loadcount);

    if(newmessages.isEmpty())
        return 0;

    this->beginInsertRows(QModelIndex(), this->_messages.count(), (this->_messages.count() + newmessages.count()) - 1);

    for(int i = 0; i < newmessages.count() - 1; i++)
        this->_messages.append(newmessages[i]);

    this->endInsertRows();
    return newmessages.count();
}

void MessagesModel::sendMessage(const QString &text)
{
    this->sendMessage(text, 0);
}

void MessagesModel::replyMessage(const QString &text, Message *replymessage)
{
    if(!replymessage)
        return;

    this->sendMessage(text, replymessage->id());
}

void MessagesModel::editMessage(const QString& text, Message* editmessage)
{
    if(!editmessage)
        return;

    this->createInputPeer();
    TelegramAPI::messagesEditMessage(DC_MainSession, this->_inputpeer, editmessage->id(), ToTLString(text), NULL, TLVector<MessageEntity*>());
}

void MessagesModel::sendAction(int action)
{
    if(!this->_telegram || !this->_dialog || (this->_timaction && this->_timaction->isActive()))
        return;

    TLConstructor actionctor = this->getAction(action);

    if(!actionctor)
        return;

    SendMessageAction sendmessageaction;
    sendmessageaction.setConstructorId(actionctor);

    this->createInputPeer();

    if(!this->_timaction)
    {
        this->_timaction = new QTimer(this);
        connect(this->_timaction, &QTimer::timeout, this, &MessagesModel::resetAction);

        this->_timaction->setSingleShot(true);
        this->_timaction->setInterval(StatusTimeout);
    }

    this->_timaction->start();
    TelegramAPI::messagesSetTyping(DC_MainSession, this->_inputpeer, &sendmessageaction);
}

void MessagesModel::onMessagesGetHistoryReplied(MTProtoReply *mtreply)
{
    MessagesMessages messages;
    messages.read(mtreply);

    if(messages.constructorId() == TLTypes::MessagesMessages)
        this->_fetchmore = false;
    else
        this->_fetchmore = (this->_messages.count() < messages.count());

    TelegramCache_store(messages.users());
    TelegramCache_store(messages.chats());
    TelegramCache_store(messages.messages());

    this->loadHistoryFromCache();
    this->setLoading(false);
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

    TelegramCache_insert(message);
}

void MessagesModel::onMessagesReadHistoryReplied(MTProtoReply *mtreply)
{
    Q_UNUSED(mtreply)
    TelegramCache_markAsRead(this->_dialog, this->inboxMaxId(), this->outboxMaxId());
}

void MessagesModel::onReadHistory(Dialog *dialog)
{
    if(!this->_dialog || !dialog || (this->_dialog != dialog))
        return;

    int idx = this->indexOf(this->_dialog->readOutboxMaxId());
    TelegramCache_markAsRead(this->_dialog, this->inboxMaxId(), this->outboxMaxId());

    if(idx > -1)
        Emit_DataChangedRangeRoles(0, idx, MessagesModel::IsMessageUnreadRole);
    else
        qWarning("Cannot find message id %d", this->_dialog->readOutboxMaxId());
}

void MessagesModel::onSendStatusUpdated(Dialog *dialog)
{
    if(this->_dialog != dialog)
        return;

    emit statusTextChanged();
}

void MessagesModel::onNewMessage(Message *message)
{
    if(!this->ownMessage(message))
        return;

    this->beginInsertRows(QModelIndex(), 0, 0);
    this->_messages.prepend(message);
    this->endInsertRows();

    this->markAsRead();
}

void MessagesModel::onEditMessage(Message *message)
{
    if(!this->ownMessage(message))
        return;

    int idx = this->indexOf(message);

    if(idx == -1)
        return;

    this->_messages[idx] = message;
    Emit_DataChanged(idx);
}

void MessagesModel::onDeleteMessage(Message* message)
{
    if(!this->ownMessage(message))
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

void MessagesModel::resetAction()
{
    SendMessageAction sendmessageaction;
    sendmessageaction.setConstructorId(TLTypes::SendMessageCancelAction);

    TelegramAPI::messagesSetTyping(DC_MainSession, this->_inputpeer, &sendmessageaction);
}

TLConstructor MessagesModel::getAction(int action)
{
    switch(action)
    {
        case MessagesModel::TypingAction:
            return TLTypes::SendMessageTypingAction;

        case MessagesModel::CancelAction:
            return TLTypes::SendMessageCancelAction;

        case MessagesModel::RecordVideoAction:
            return TLTypes::SendMessageRecordVideoAction;

        case MessagesModel::UploadVideoAction:
            return TLTypes::SendMessageUploadVideoAction;

        case MessagesModel::RecordAudioAction:
            return TLTypes::SendMessageRecordAudioAction;

        case MessagesModel::UploadAudioAction:
            return TLTypes::SendMessageUploadAudioAction;

        case MessagesModel::UploadPhotoAction:
            return TLTypes::SendMessageUploadPhotoAction;

        case MessagesModel::UploadDocumentAction:
            return TLTypes::SendMessageUploadDocumentAction;

        case MessagesModel::GeoLocationAction:
            return TLTypes::SendMessageGeoLocationAction;

        case MessagesModel::ChooseContactAction:
            return TLTypes::SendMessageChooseContactAction;

        case MessagesModel::GamePlayAction:
            return TLTypes::SendMessageGamePlayAction;

        case MessagesModel::GameStopAction:
            return TLTypes::SendMessageGameStopAction;

        default:
            break;
    }

    return 0;
}

void MessagesModel::sendMessage(const QString &text, TLInt replymsgid)
{
    if(!this->_dialog || text.trimmed().isEmpty())
        return;

    this->createInputPeer();

    TLLong randomid = 0;
    Math::randomize(&randomid);

    Message* message = TelegramHelper::createMessage(text, TelegramConfig_me, this->_dialog->peer());

    if(replymsgid)
        message->setReplyToMsgId(replymsgid);

    this->_pendingmessages << message;

    MTProtoRequest* req = TelegramAPI::messagesSendMessage(DC_MainSession, this->_inputpeer, replymsgid, text.trimmed(), randomid, NULL, TLVector<MessageEntity*>());
    connect(req, &MTProtoRequest::replied, this, &MessagesModel::onMessagesSendMessageReplied);
}

void MessagesModel::setFirstNewMessage()
{
    TLInt maxinmsgid = this->_dialog->readInboxMaxId();

    if(maxinmsgid >= this->_messages.first()->id())
        return;

    for(int i = 0; i < this->_messages.length(); i++)
    {
        Message* message = this->_messages[i];

        if(message->isOut() || (message->id() <= maxinmsgid))
            return;

        this->_newmessageindex = i;
        this->_newmessageid = message->id();
    }

    if(this->_newmessageindex != -1)
    {
        emit newMessageIndexChanged();
        Emit_DataChangedRoles(this->_newmessageindex, MessagesModel::IsMessageNewRole);
    }
}

TLInt MessagesModel::inboxMaxId() const
{
    for(int i = 0; i < this->_messages.length(); i++)
    {
        Message* message = this->_messages[i];

        if(message->isOut())
            continue;

        return message->id();
    }

    return this->_dialog->readInboxMaxId();
}

TLInt MessagesModel::outboxMaxId() const
{
    for(int i = 0; i < this->_messages.length(); i++)
    {
        Message* message = this->_messages[i];

        if(!message->isOut())
            continue;

        return message->id();
    }

    return this->_dialog->readOutboxMaxId();
}

void MessagesModel::markAsRead()
{
    if(this->_dialog->unreadCount() <= 0)
        return;

    this->createInputPeer();
    MTProtoRequest* req = TelegramAPI::messagesReadHistory(DC_MainSession, this->_inputpeer, this->_messages.first()->id());
    connect(req, &MTProtoRequest::replied, this, &MessagesModel::onMessagesReadHistoryReplied);
}

int MessagesModel::indexOf(Message *message) const
{
    return this->indexOf(message->id());
}

int MessagesModel::indexOf(TLInt messageid) const
{
    for(int i = 0; i < this->_messages.count(); i++)
    {
        if(this->_messages[i]->id() == messageid)
            return i;
    }

    return -1;
}

bool MessagesModel::ownMessage(Message *message) const
{
    return this->_dialog && (TelegramHelper::identifier(this->_dialog) == TelegramHelper::messageToDialog(message));
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

void MessagesModel::createInputPeer()
{
    if(this->_inputpeer)
        return;

    this->_inputpeer = this->_telegram->createInputPeer(this->_dialog, this);
}

void MessagesModel::telegramReady()
{
    if(!this->_dialog)
        return;

    connect(TelegramCache_instance, &TelegramCache::newMessage, this, &MessagesModel::onNewMessage);
    connect(TelegramCache_instance, &TelegramCache::deleteMessage, this, &MessagesModel::onDeleteMessage);
    connect(TelegramCache_instance, &TelegramCache::editMessage, this, &MessagesModel::onEditMessage);
    connect(TelegramCache_instance, &TelegramCache::readHistory, this, &MessagesModel::onReadHistory);

    connect(SendStatusHandler_instance, &SendStatusHandler::sendStatusUpdated, this, &MessagesModel::onSendStatusUpdated);

    if(TelegramHelper::isChannel(this->_dialog) || (TelegramHelper::isChat(this->_dialog)))
    {
        Chat* chat = TelegramCache_chat(TelegramHelper::identifier(this->_dialog));
        connect(chat, &Chat::participantsCountChanged, this, &MessagesModel::statusTextChanged);
    }
    else
    {
        User* user = TelegramCache_user(TelegramHelper::identifier(this->_dialog));
        connect(user, &User::statusChanged, this, &MessagesModel::statusTextChanged);
    }

    this->_messages = TelegramCache_lastDialogMessages(this->_dialog);

    if(this->_messages.isEmpty())
        return;

    this->beginInsertRows(QModelIndex(), 0, this->_messages.count() - 1);
    this->endInsertRows();

    this->setFirstNewMessage();
    this->markAsRead();
}
