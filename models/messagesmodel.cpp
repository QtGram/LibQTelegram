#include "messagesmodel.h"
#include "../cache/telegramcache.h"
#include "../objects/sendstatus/sendstatushandler.h"
#include "../crypto/math.h"
#include <QImageReader>

#define MessagesFirstLoad 30
#define MessagesPerPage   50

MessagesModel::MessagesModel(QObject *parent) : TelegramModel(parent)
{
    this->_inputpeer = this->_migratedinputpeer = NULL;
    this->_inputchannel = NULL;
    this->_dialog = NULL;
    this->_timaction = NULL;
    this->_newmessageid = this->_lastreadedinbox = this->_migratedfromchatid = 0;
    this->_newmessageindex = this->_migrationmessageindex = -1;
    this->_isactive = this->_fetchmore = true;
    this->_loadcount = MessagesFirstLoad;

    connect(this, &MessagesModel::dialogChanged, this, &MessagesModel::titleChanged);
    connect(this, &MessagesModel::dialogChanged, this, &MessagesModel::statusTextChanged);
    connect(this, &MessagesModel::dialogChanged, this, &MessagesModel::isChatChanged);
    connect(this, &MessagesModel::dialogChanged, this, &MessagesModel::isBroadcastChanged);
    connect(this, &MessagesModel::dialogChanged, this, &MessagesModel::isMegaGroupChanged);
    connect(this, &MessagesModel::dialogChanged, this, &MessagesModel::isWritableChanged);
    connect(this, &MessagesModel::isActiveChanged, this, &MessagesModel::markAsRead);
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

void MessagesModel::setIsActive(bool isactive)
{
    if(this->_isactive == isactive)
        return;

    this->_isactive = isactive;
    emit isActiveChanged();
}

void MessagesModel::setIsMuted(bool ismuted)
{
    if(!this->_telegram || !this->_dialog || (TelegramHelper::isMuted(this->_dialog) == ismuted))
        return;

    this->_telegram->muteDialog(this->_dialog, ismuted);
    emit isMutedChanged();
}

Message *MessagesModel::get(int index) const
{
    if (index >= 0 && index < _messages.count()) {
        return _messages.at(index);
    }
    return nullptr;
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
    if(!this->_telegram || !this->_dialog)
        return false;

    return this->_telegram->dialogIsWritable(this->_dialog);
}

bool MessagesModel::isActive() const
{
    return this->_isactive;
}

bool MessagesModel::isMuted() const
{
    if(!this->_dialog)
        return false;

    return TelegramHelper::isMuted(this->_dialog);
}

bool MessagesModel::canFetchMore(const QModelIndex &) const
{
    if(!this->_telegram || (this->_telegram && this->_telegram->syncing()))
        return false;

    return this->_dialog && this->_fetchmore && !this->_loading;
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
        this->terminateInitialization();
        this->setLoading(false);
        return;
    }

    this->createInput();

    MTProtoRequest* req = NULL;

    if(!this->_migratedinputpeer)
        req = TelegramAPI::messagesGetHistory(DC_MainSession, this->_inputpeer, 0, 0, this->_messages.count(), this->_loadcount, 0, 0);
    else
        req = TelegramAPI::messagesGetHistory(DC_MainSession, this->_migratedinputpeer, 0, 0, this->_messages.count() - this->_migrationmessageindex, this->_loadcount, 0, 0);

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

    if(role == MessagesModel::MessageCaptionRole)
    {
        if(!message->media())
            return QString();

        return message->media()->caption().toString();
    }

    if(role == MessagesModel::ReplyItemRole)
        return message->replyToMsgId() ? QVariant::fromValue(TelegramCache_message(message->replyToMsgId(), this->_dialog)) : QVariant();

    if(role == MessagesModel::ReplyFromRole)
    {
        if(message->replyToMsgId() == 0)
            return QString();

        return this->messageFrom(TelegramCache_message(message->replyToMsgId(), this->_dialog));
    }

    if(role == MessagesModel::ReplyTextRole)
    {
        if(message->replyToMsgId() == 0)
            return QString();

        Message* replymessage = TelegramCache_message(message->replyToMsgId(), this->_dialog);

        if(replymessage && replymessage->media() && (replymessage->media()->constructorId() != TLTypes::MessageMediaEmpty))
            return this->_telegram->messagePreview(replymessage);

        return this->_telegram->messageText(replymessage);
    }

    if(role == MessagesModel::ReplyCaptionRole)
    {
        if(message->replyToMsgId() == 0)
            return QString();

        Message* replymessage = TelegramCache_message(message->replyToMsgId(), this->_dialog);

        if(replymessage && replymessage->media() && (replymessage->media()->constructorId() != TLTypes::MessageMediaEmpty))
            return this->_telegram->messagePreview(replymessage);

        return QString();
    }

    if(role == MessagesModel::IsMessageNewRole)
    {
        if(this->_dialog->unreadCount() <= 0)
            return false;

        return message->id() == this->_newmessageid;
    }

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
    {
        if((this->_migrationmessageindex != -1) && (index.row() >= this->_migrationmessageindex))
            return false;

        return this->_dialog->readOutboxMaxId() < message->id();
    }

    if(role == MessagesModel::IsMessageEditedRole)
        return message->editDate() != 0;

    if(role == MessagesModel::IsMessagePendingRole)
        return is_local_messageid(message->id());

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

    if(role == MessagesModel::ForwardedFromPeerRole)
    {
        if(!message->fwdFrom())
            return QVariant();

        if(message->fwdFrom()->fromId())
            return QVariant::fromValue(TelegramCache_user(message->fwdFrom()->fromId()));
        else if(message->fwdFrom()->channelId())
            return QVariant::fromValue(TelegramCache_chat(message->fwdFrom()->channelId()));

        return QVariant();
    }

    if(role == MessagesModel::ForwardedFromNameRole)
    {
        if(!message->fwdFrom())
            return QVariant();

        if(message->fwdFrom()->fromId())
        {
            User* user = TelegramCache_user(message->fwdFrom()->fromId());

            if(user)
                return TelegramHelper::fullName(user);
        }
        else if(message->fwdFrom()->channelId())
        {
            Chat* chat = TelegramCache_chat(message->fwdFrom()->channelId());

            if(chat)
                return chat->title().toString();
        }
    }

    return QVariant();
}

int MessagesModel::rowCount(const QModelIndex &) const
{
    if(this->_initializing)
        return 0;

    return this->_messages.length();
}

QHash<int, QByteArray> MessagesModel::roleNames() const
{
    QHash<int, QByteArray> roles = this->initRoles();

    roles[MessagesModel::MessageFromRole] = "messageFrom";
    roles[MessagesModel::MessageTextRole] = "messageText";
    roles[MessagesModel::MessageCaptionRole] = "messageCaption";
    roles[MessagesModel::MessageDateRole] = "messageDate";
    roles[MessagesModel::MessageHasReplyRole] = "messageHasReply";
    roles[MessagesModel::ReplyItemRole] = "replyItem";
    roles[MessagesModel::ReplyFromRole] = "replyFrom";
    roles[MessagesModel::ReplyTextRole] = "replyText";
    roles[MessagesModel::ReplyCaptionRole] = "replyCaption";
    roles[MessagesModel::IsMessageForwardedRole] = "isMessageForwarded";
    roles[MessagesModel::IsMessageNewRole] = "isMessageNew";
    roles[MessagesModel::IsMessageOutRole] = "isMessageOut";
    roles[MessagesModel::IsMessageServiceRole] = "isMessageService";
    roles[MessagesModel::IsMessageMediaRole] = "isMessageMedia";
    roles[MessagesModel::IsMessageUnreadRole] = "isMessageUnread";
    roles[MessagesModel::IsMessageEditedRole] = "isMessageEdited";
    roles[MessagesModel::IsMessagePendingRole] = "isMessagePending";
    roles[MessagesModel::NeedsPeerImageRole] = "needsPeerImage";
    roles[MessagesModel::ForwardedFromPeerRole] = "forwardedFromPeer";
    roles[MessagesModel::ForwardedFromNameRole] = "forwardedFromName";

    return roles;
}

int MessagesModel::loadHistoryFromCache()
{
    bool hasmigration = false;
    QList<Message*> newmessages;

    if(!this->_migratedfromchatid) // Take care of old group's messages, if any
        newmessages = TelegramCache_messages(TelegramHelper::identifier(this->_dialog), this->_messages.count(), this->_loadcount, &hasmigration);
    else
        newmessages = TelegramCache_messages(this->_migratedfromchatid, this->_messages.count() - this->_migrationmessageindex, this->_loadcount, &hasmigration);

    if(newmessages.isEmpty())
        return 0;

    int count = newmessages.count();

    if(hasmigration)
        count--;

    TLInt chatid = 0;
    this->beginInsertRows(QModelIndex(), this->_messages.count(), (this->_messages.count() + count) - 1);

    for(int i = 0; i < newmessages.count(); i++)
    {
        if((chatid = TelegramHelper::messageIsMigratedFrom(newmessages[i])))
        {
            this->_migratedfromchatid = chatid;
            this->_migrationmessageindex = this->_messages.length();
            continue;
        }

        this->_messages.append(newmessages[i]);
    }

    this->endInsertRows();
    return count;
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

void MessagesModel::forwardMessages(Dialog* todialog, const QVariantList& messages)
{
    if(!this->_telegram || !this->_dialog || !todialog || messages.isEmpty())
        return;

    InputPeer* topeer = TelegramHelper::inputPeer(todialog, TelegramCache_accessHash(todialog), this);

    TLVector<TLInt> msgids;
    TLVector<TLLong> randomids;

    foreach(QVariant v, messages)
    {
        Message* message = v.value<Message*>();

        msgids << message->id();
        randomids << Math::randomize<TLLong>();
    }

    std::sort(msgids.begin(), msgids.end(), std::less<TLInt>());

    this->createInput();
    TelegramAPI::messagesForwardMessages(DC_MainSession, this->_inputpeer, msgids, randomids, topeer);
    topeer->deleteLater();
}

void MessagesModel::deleteMessages(const QVariantList &messages)
{
    if(!this->_telegram || !this->_dialog || messages.isEmpty())
        return;

    TLVector<TLInt> msgids;

    foreach(QVariant v, messages)
    {
        Message* message = v.value<Message*>();
        msgids << message->id();

        TelegramCache_remove(message);
    }

    std::sort(msgids.begin(), msgids.end(), std::less<TLInt>());

    if(TelegramHelper::isChannel(this->_dialog))
    {
        this->createInput();
        TelegramAPI::channelsDeleteMessages(DC_MainSession, this->_inputchannel, msgids);
        return;
    }

    TelegramAPI::messagesDeleteMessages(DC_MainSession, msgids);
}

void MessagesModel::editMessage(const QString& text, Message* editmessage)
{
    if(!editmessage || text.isEmpty())
        return;

    this->createInput();
    TelegramAPI::messagesEditMessage(DC_MainSession, this->_inputpeer, editmessage->id(), ToTLString(text), NULL, TLVector<MessageEntity*>());
}

void MessagesModel::sendFile(const QUrl &filepath, const QString& caption)
{
    this->sendMedia(filepath, caption, FileUploader::MediaDocument);
}

bool MessagesModel::sendPhoto(const QUrl &filepath, const QString &caption)
{
    if(QImageReader::imageFormat(filepath.toLocalFile()).isEmpty())
    {
        qWarning("Invalid image: %s", qUtf8Printable(filepath.toLocalFile()));
        return false;
    }

    this->sendMedia(filepath, caption, FileUploader::MediaPhoto);
    return true;
}

void MessagesModel::sendLocation(TLDouble latitude, TLDouble longitude)
{
    InputMedia* inputgeopoint = TelegramHelper::inputMedia(latitude, longitude);

    this->createInput();

    // NOTE: Duplicate call
    TelegramAPI::messagesSendMedia(DC_MainSession,
                                   this->_inputpeer,
                                   0, // TODO: Handle reply_to_msgid
                                   inputgeopoint,
                                   Math::randomize<TLLong>(),
                                   NULL);

    inputgeopoint->deleteLater();
}

void MessagesModel::sendSticker(Document *sticker)
{
    Message* message = TelegramHelper::createMessage(sticker, TelegramConfig_me, this->_dialog->peer(), this);
    InputMedia* inputmedia = TelegramHelper::inputMedia(sticker);

    this->createInput();

    MTProtoRequest* req = TelegramAPI::messagesSendMedia(DC_MainSession,
                                                         this->_inputpeer,
                                                         message->replyToMsgId(),
                                                         inputmedia,
                                                         Math::randomize<TLLong>(),
                                                         NULL);

    this->insertMessage(local_messageid(req->requestId()), message);
    connect(req, &MTProtoRequest::replied, this, &MessagesModel::onMessagesSendMediaReplied);
    inputmedia->deleteLater();
}

void MessagesModel::sendAction(int action)
{
    if(!this->_telegram || !this->_dialog || (this->_timaction && this->_timaction->isActive()))
        return;

    if(!this->_telegram->connected() || TelegramHelper::isCloud(this->_dialog)) // Don't send status when not connected and don't notify myself
        return;

    if(TelegramHelper::isUser(this->_dialog))
    {
        TLInt dialogid = TelegramHelper::identifier(this->_dialog);
        User* user = TelegramCache_user(dialogid);

        if(user && !TelegramHelper::userIsOnline(user))
            return;
    }

    TLConstructor actionctor = this->getAction(action);

    if(!actionctor)
        return;

    SendMessageAction sendmessageaction;
    sendmessageaction.setConstructorId(actionctor);

    this->createInput();

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

void MessagesModel::onUploadCompleted()
{
    FileObject* fileobject = qobject_cast<FileObject*>(this->sender());
    this->createInput();

    MTProtoRequest* req = TelegramAPI::messagesSendMedia(DC_MainSession,
                                                         this->_inputpeer,
                                                         0, // TODO: Handle reply_to_msgid
                                                         fileobject->inputMedia(),
                                                         Math::randomize<TLLong>(),
                                                         NULL);

    Message* message = this->_pendingmessages[local_messageid(fileobject->uploader()->localFileId())];
    this->updateMessage(local_messageid(req->requestId()), message);

    connect(req, &MTProtoRequest::replied, this, &MessagesModel::onMessagesSendMediaReplied);
}

void MessagesModel::onMessagesGetHistoryReplied(MTProtoReply *mtreply)
{
    MessagesMessages messages;
    messages.read(mtreply);

    TelegramCache_store(messages.users());
    TelegramCache_store(messages.chats());
    TelegramCache_store(messages.messages());

    this->loadHistoryFromCache();

    if(messages.constructorId() == TLTypes::MessagesMessages)
        this->_fetchmore = false;
    else
        this->_fetchmore = (this->_messages.count() < messages.count());

    this->terminateInitialization();
    this->setLoading(false);
}

void MessagesModel::onMessagesSendMessageReplied(MTProtoReply *mtreply)
{
    TLInt tempmsgid = local_messageid(mtreply->requestId());

    if(!this->_pendingmessages.contains(tempmsgid))
    {
        qWarning("Cannot find pending message with temporary-id %d", tempmsgid);
        return;
    }

    Updates updates;
    updates.read(mtreply);

    Q_ASSERT((updates.constructorId() == TLTypes::UpdateShortSentMessage) ||
             (updates.constructorId() == TLTypes::Updates));

    Q_ASSERT(this->_pendingmessages.contains(tempmsgid));
    Message* message = this->_pendingmessages.take(tempmsgid);

    if(updates.constructorId() == TLTypes::UpdateShortSentMessage)
    {
        message->setId(updates.id());
        message->setFlags(updates.flags());
        message->setMedia(updates.media());

        this->_pendingmessages[message->id()] = message; // Keep it in order to avoid double insertions
        TelegramCache_insert(message);
        return;
    }

    Update* update = updates.updates().last();
    Q_ASSERT(update->constructorId() == TLTypes::UpdateNewChannelMessage);

    message->deleteLater();

    this->beginRemoveRows(QModelIndex(), 1, 1);
    this->_messages.removeAt(1);
    this->endRemoveRows();
}

void MessagesModel::onMessagesSendMediaReplied(MTProtoReply *mtreply)
{
    TLInt tempmsgid = local_messageid(mtreply->requestId());

    if(!this->_pendingmessages.contains(tempmsgid))
    {
        qWarning("Cannot find pending media message with temporary-id %d", tempmsgid);
        return;
    }

    Message* message = this->_pendingmessages.take(tempmsgid);
    int idx = this->_messages.indexOf(message);

    if(idx == -1)
    {
        qWarning("Cannot find media message %d", message->id());
        return;
    }

    this->beginRemoveRows(QModelIndex(), idx, idx);
    this->_messages.removeAt(idx);
    this->endRemoveRows();

    message->deleteLater();
}

void MessagesModel::onReadHistoryReplied(MTProtoReply *mtreply)
{
    Q_UNUSED(mtreply)
    TelegramCache_markAsRead(this->_dialog, this->inboxMaxId(), this->_dialog->readOutboxMaxId());
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

void MessagesModel::onTitleChanged(Dialog *dialog)
{
    if(this->_dialog != dialog)
        return;

    emit titleChanged();
}

void MessagesModel::updateStatusText(Dialog *dialog)
{
    if(this->_dialog != dialog)
        return;

    emit statusTextChanged();
}

void MessagesModel::onNewMessage(Message *message)
{
    if(!this->ownMessage(message))
        return;

    int idx = -1;

    if(this->_pendingmessages.contains(message->id()))
    {
        this->_pendingmessages.remove(message->id());
        idx = this->_messages.indexOf(message);

        if(idx != -1)
        {
            this->beginRemoveRows(QModelIndex(), idx, idx);
            this->_messages.removeAt(idx);
            this->endRemoveRows();
        }
        else
            qWarning("Cannot find message %d", message->id());
    }

    idx = this->insertionPoint(message);

    this->beginInsertRows(QModelIndex(), idx, idx);
    this->_messages.insert(idx, message);
    this->endInsertRows();

    if(TelegramHelper::messageIsMigratedFrom(message))
        this->_migrationmessageindex = this->_messages.length() - 1;

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

void MessagesModel::onUpdateMessage(Message *message)
{
    if(this->_messages.isEmpty() || !this->ownMessage(message))
        return;

    Message* oldestmessage = this->_messages.last();

    if(message->id() < oldestmessage->id()) // We don't have this message loaded
        return;

    int idx = this->indexOf(message);

    if(idx == -1)
    {
        qWarning("Cannot update message %d", message->id());
        return;
    }

    Emit_DataChangedRoles(idx, MessagesModel::ItemRole <<
                               MessagesModel::IsMessageMediaRole);
}

void MessagesModel::onNotifySettingsChanged(Dialog *dialog)
{
    if(this->_dialog != dialog)
        return;

    emit isMutedChanged();
}

void MessagesModel::resetAction()
{
    SendMessageAction sendmessageaction;
    sendmessageaction.setConstructorId(TLTypes::SendMessageCancelAction);

    TelegramAPI::messagesSetTyping(DC_MainSession, this->_inputpeer, &sendmessageaction);
}

TLConstructor MessagesModel::getAction(int action) const
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

int MessagesModel::insertionPoint(Message *message) const
{
    for(int i = 0; i < this->_messages.length(); i++)
    {
        if(this->_messages[i]->id() < message->id())
            return i;
    }

    return this->_messages.length() - 1;
}

void MessagesModel::sendMessage(const QString &text, TLInt replymsgid)
{
    if(!this->_dialog || text.trimmed().isEmpty())
        return;

    Message* message = TelegramHelper::createMessage(text, TelegramConfig_me, this->_dialog->peer(), this);

    if(replymsgid)
        message->setReplyToMsgId(replymsgid);

    this->createInput();

    MTProtoRequest* req = TelegramAPI::messagesSendMessage(DC_MainSession,
                                                           this->_inputpeer,
                                                           message->replyToMsgId(),
                                                           message->message(),
                                                           Math::randomize<TLLong>(),
                                                           NULL,
                                                           TLVector<MessageEntity*>());

    this->insertMessage(local_messageid(req->requestId()), message);
    connect(req, &MTProtoRequest::replied, this, &MessagesModel::onMessagesSendMessageReplied);
}

void MessagesModel::updateMessage(TLInt localmessageid, Message *message)
{
    message->setId(localmessageid);
    this->_pendingmessages[localmessageid] = message;
}

void MessagesModel::insertMessage(TLInt localmessageid, Message *message)
{
    this->updateMessage(localmessageid, message);

    this->beginInsertRows(QModelIndex(), 0, 0);
    this->_messages.insert(0, message);
    this->endInsertRows();
}

void MessagesModel::setFirstNewMessage()
{
    if(this->_messages.isEmpty())
        return;

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

        if(message->isOut() || is_local_messageid(message->id()))
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

        if(!message->isOut() || is_local_messageid(message->id()))
            continue;

        return message->id();
    }

    return this->_dialog->readOutboxMaxId();
}

void MessagesModel::markAsRead()
{
    if(this->_initializing || !this->_isactive || !this->_dialog)
        return;

    TLInt inboxmaxid = this->inboxMaxId();

    if(this->_lastreadedinbox >= inboxmaxid)
        return;

    this->_lastreadedinbox = inboxmaxid;

    MTProtoRequest* req = NULL;
    this->createInput();

    if(TelegramHelper::isChannel(this->_dialog))
        req = TelegramAPI::channelsReadHistory(DC_MainSession, this->_inputchannel, inboxmaxid);
    else
        req = TelegramAPI::messagesReadHistory(DC_MainSession, this->_inputpeer, inboxmaxid);

    connect(req, &MTProtoRequest::replied, this, &MessagesModel::onReadHistoryReplied);
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

void MessagesModel::sendMedia(const QUrl &filepath, const QString &caption, FileUploader::MediaType mediatype)
{
    if(!this->_telegram || !this->_dialog || !QFile::exists(filepath.toLocalFile()))
        return;

    FileObject* fileobject = FileCache_upload(mediatype, filepath.toLocalFile(), caption);
    connect(fileobject, &FileObject::uploadCompleted, this, &MessagesModel::onUploadCompleted);

    Message* message = TelegramHelper::createMessage(QString(), TelegramConfig_me, this->_dialog->peer(), this);
    message->setMedia(fileobject->messageMedia());

    this->insertMessage(local_messageid(fileobject->uploader()->localFileId()), message);
}

void MessagesModel::createInput()
{
    if(!this->_dialog)
        return;

    if(!this->_inputchannel && TelegramHelper::isChannel(this->_dialog))
        this->_inputchannel = TelegramHelper::inputChannel(this->_dialog, TelegramCache_accessHash(this->_dialog), this);

    if(!this->_inputpeer)
        this->_inputpeer = TelegramHelper::inputPeer(this->_dialog, TelegramCache_accessHash(this->_dialog), this);

    if(!this->_migratedinputpeer && this->_migratedfromchatid)
    {
        Chat* chat = TelegramCache_chat(this->_migratedfromchatid);

        if(chat)
            this->_migratedinputpeer = TelegramHelper::inputPeer(chat, this);
    }
}

void MessagesModel::terminateInitialization()
{
    if(!this->_initializing)
        return;

    this->beginResetModel();
    this->setInitializing(false);
    this->endResetModel();
}

void MessagesModel::telegramReady()
{
    if(!this->_dialog || !this->_telegram)
        return;

    if(this->_telegram->syncing())
    {
        connect(this->_telegram, &Telegram::syncingChanged, this, &MessagesModel::telegramReady);
        return;
    }

    disconnect(this->_telegram, &Telegram::syncingChanged, this, 0);

    connect(TelegramCache_instance, &TelegramCache::newMessage, this, &MessagesModel::onNewMessage);
    connect(TelegramCache_instance, &TelegramCache::deleteMessage, this, &MessagesModel::onDeleteMessage);
    connect(TelegramCache_instance, &TelegramCache::editMessage, this, &MessagesModel::onEditMessage);
    connect(TelegramCache_instance, &TelegramCache::readHistory, this, &MessagesModel::onReadHistory);
    connect(TelegramCache_instance, &TelegramCache::titleChanged, this, &MessagesModel::onTitleChanged);
    connect(TelegramCache_instance, &TelegramCache::chatFullChanged, this, &MessagesModel::updateStatusText);
    connect(TelegramCache_instance, &TelegramCache::dialogNotifySettingsChanged, this, &MessagesModel::onNotifySettingsChanged, Qt::UniqueConnection);

    connect(SendStatusHandler_instance, &SendStatusHandler::sendStatusUpdated, this, &MessagesModel::updateStatusText);

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

    this->setInitializing(true);
    this->_messages = TelegramCache_lastDialogMessages(this->_dialog);

    if(this->_messages.count() >= MessagesFirstLoad)
        this->terminateInitialization();
    else
        this->fetchMore();

    this->setFirstNewMessage();
    this->markAsRead();
}
