#include "dialogsmodel.h"
#include "../cache/telegramcache.h"
#include "../objects/sendstatus/sendstatushandler.h"

DialogsModel::DialogsModel(QObject *parent) : TelegramModel(parent), _clouddialog(NULL)
{
}

Dialog *DialogsModel::cloudDialog()
{
    TLInt dialogid = TelegramHelper::identifier(TelegramConfig_me);

    if(TelegramCache_hasDialog(dialogid))
        return TelegramCache_dialog(dialogid);

    if(!this->_clouddialog)
        this->_clouddialog = TelegramHelper::createDialog(TelegramConfig_me, this);

    return this->_clouddialog;
}

QVariant DialogsModel::data(const QModelIndex &index, int role) const
{
    if(!index.isValid() || (index.row() >= this->_dialogs.count()))
        return QVariant();

    Dialog* dialog = this->_dialogs[index.row()];

    if(role == DialogsModel::ItemRole)
        return QVariant::fromValue(dialog);

    if(role == DialogsModel::TitleRole)
        return this->_telegram->dialogTitle(dialog);

    if(role == DialogsModel::UnreadCountRole)
        return dialog->unreadCount();

    if(role == DialogsModel::PeerActionRole)
        return SendStatusHandler_dialogSendStatus(dialog);

    if(role == DialogsModel::TopMessageRole)
    {
        Message* message = TelegramCache_message(dialog->topMessage(), dialog);

        if(!message)
            return QVariant();

        return QVariant::fromValue(message);
    }

    if(role == DialogsModel::TopMessageFromRole)
    {
        Message* message = TelegramCache_message(dialog->topMessage(), dialog);

        if(!message)
            return QVariant();

        return this->messageFrom(message);
    }

    if(role == DialogsModel::TopMessageTextRole)
    {
        if(!dialog->topMessage())
            return QString();

        Message* message = TelegramCache_message(dialog->topMessage(), dialog);

        if(!message)
            return QVariant();

        return this->firstMessageLine(message);
    }

    if(role == DialogsModel::TopMessageDateRole)
    {
        if(!dialog->topMessage())
            return QString();

        Message* message = TelegramCache_message(dialog->topMessage(), dialog);

        if(!message)
            return QVariant();

        return TelegramHelper::dateString(message->date());
    }

    if(role == DialogsModel::IsTopMessageUnreadRole)
    {
        if(!dialog->topMessage())
            return false;

        Message* message = TelegramCache_message(dialog->topMessage(), dialog);

        if(!message)
            return false;

        return (dialog->readOutboxMaxId() < message->id());
    }

    if(role == DialogsModel::IsTopMessageOutRole)
    {
        if(!dialog->topMessage())
            return false;

        Message* message = TelegramCache_message(dialog->topMessage(), dialog);

        if(!message)
            return false;

        return message->isOut();
    }

    if(role == DialogsModel::IsTopMessageServiceRole)
    {
        if(!dialog->topMessage())
            return false;

        Message* message = TelegramCache_message(dialog->topMessage(), dialog);

        if(!message)
            return false;

        return message->constructorId() == TLTypes::MessageService;
    }

    if(role == DialogsModel::DraftMessageRole)
        return this->draftMessage(dialog);

    if(role == DialogsModel::IsChatRole)
        return TelegramHelper::isChat(dialog);

    if(role == DialogsModel::IsCloudRole)
        return TelegramHelper::isCloud(dialog);

    if(role == DialogsModel::IsBroadcastRole)
    {
        if(!TelegramHelper::isChannel(dialog))
            return false;

        Chat* chat = TelegramCache_chat(TelegramHelper::identifier(dialog));

        if(!chat)
            return false;

        return chat->isBroadcast();
    }

    if(role == DialogsModel::IsMegaGroupRole)
    {
        if(!TelegramHelper::isChannel(dialog))
            return false;

        Chat* chat = TelegramCache_chat(TelegramHelper::identifier(dialog));

        if(!chat)
            return false;

        return chat->isMegagroup();
    }

    if(role == DialogsModel::IsMutedRole)
        return TelegramHelper::isMuted(dialog);

    return QVariant();
}

bool DialogsModel::setData(const QModelIndex &index, const QVariant &value, int role)
{
    if(!this->_telegram || !index.isValid() || (index.row() >= this->_dialogs.count()))
        return false;

    Dialog* dialog = this->_dialogs[index.row()];

    if(role == DialogsModel::IsMutedRole)
    {
        bool done = this->_telegram->muteDialog(dialog, value.toBool());

        if(done)
            Emit_DataChangedRoles(index.row(), DialogsModel::IsMutedRole);

        return done;
    }

    return false;
}

int DialogsModel::rowCount(const QModelIndex &) const
{
    return this->_dialogs.length();
}

QHash<int, QByteArray> DialogsModel::roleNames() const
{
    QHash<int, QByteArray> roles = this->initRoles();

    roles[DialogsModel::TitleRole] = "title";
    roles[DialogsModel::DraftMessageRole] = "draftMessage";
    roles[DialogsModel::UnreadCountRole] = "unreadCount";
    roles[DialogsModel::PeerActionRole] = "peerAction";
    roles[DialogsModel::TopMessageRole] = "topMessage";
    roles[DialogsModel::TopMessageFromRole] = "topMessageFrom";
    roles[DialogsModel::TopMessageTextRole] = "topMessageText";
    roles[DialogsModel::TopMessageDateRole] = "topMessageDate";
    roles[DialogsModel::IsTopMessageUnreadRole] = "isTopMessageUnread";
    roles[DialogsModel::IsTopMessageOutRole] = "isTopMessageOut";
    roles[DialogsModel::IsTopMessageServiceRole] = "isTopMessageService";
    roles[DialogsModel::IsMegaGroupRole] = "isMegaGroup";
    roles[DialogsModel::IsBroadcastRole] = "isBroadcast";
    roles[DialogsModel::IsChatRole] = "isChat";
    roles[DialogsModel::IsCloudRole] = "isCloud";
    roles[DialogsModel::IsMutedRole] = "isMuted";

    return roles;
}

int DialogsModel::indexOf(Dialog *dialog) const
{
    return this->_dialogs.indexOf(dialog);
}

Dialog *DialogsModel::getDialog(TLInt dialogid) const
{
    foreach(Dialog* dialog, this->_dialogs)
    {
        if(TelegramHelper::identifier(dialog) == dialogid)
            return dialog;
    }

    qWarning("Cannot find dialog %d", dialogid);
    return NULL;
}

void DialogsModel::removeDialog(int index)
{
    if(!this->_telegram || (index < 0) || (index >= this->_dialogs.length()))
        return;

    Dialog* dialog = this->_dialogs[index];

    if(dialog->topMessage() > 0)
        this->clearHistory(index);

    this->doRemoveDialog(index);
}

void DialogsModel::clearHistory(int index)
{
    if(!this->_telegram || (index < 0) || (index >= this->_dialogs.length()))
        return;

    Dialog* dialog = this->_dialogs[index];

    if(dialog->topMessage() <= 0)
        return;

    TelegramCache_clearHistory(dialog);

    InputPeer* inputpeer = TelegramHelper::inputPeer(dialog, TelegramCache_accessHash(dialog), this);
    MTProtoRequest* req = TelegramAPI::messagesReadHistory(DC_MainSession, inputpeer, dialog->topMessage());

    connect(req, &MTProtoRequest::replied, [this, dialog, inputpeer, index](MTProtoReply*) {
        TelegramAPI::messagesDeleteHistory(DC_MainSession, inputpeer, dialog->topMessage());
        inputpeer->deleteLater();
    });
}

QString DialogsModel::dialogTitle(Dialog *dialog) const
{
    return this->_telegram->dialogTitle(dialog);
}

void DialogsModel::doRemoveDialog(int index)
{
    Dialog* dialog = this->_dialogs[index];
    TelegramCache_remove(dialog);
}

int DialogsModel::insertionPoint(Dialog *insdialog, int fromidx) const
{
    if(!insdialog->topMessage())
        return this->_dialogs.length() - 1;

    Message* msg1 = TelegramCache_message(insdialog->topMessage(), insdialog);

    if(!msg1)
        return this->_dialogs.length() - 1;

    for(int i = fromidx + 1; i < this->_dialogs.length(); i++)
    {
        Dialog* dialog = this->_dialogs[i];

        if((dialog == insdialog) || !dialog->topMessage())
            return i;

        Message* msg2 = TelegramCache_message(dialog->topMessage(), dialog);

        if(!msg2 || (msg1->date() > msg2->date()))
            return i;
    }

    return this->_dialogs.length() - 1;
}

int DialogsModel::indexOf(TLInt dialogid) const
{
    for(int i = 0; i < this->_dialogs.length(); i++)
    {
        if(TelegramHelper::identifier(this->_dialogs[i]) == dialogid)
            return i;
    }

    return -1;
}

QString DialogsModel::messageFrom(Message *message) const
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
            return qobject_cast<User*>(tgobj)->firstName();

        default:
            break;
    }

    return QString();
}

QString DialogsModel::firstMessageLine(Message *message) const
{
    QString preview = this->_telegram->messagePreview(message);
    int newline = preview.indexOf("\n");

    if(newline > -1)
        return preview.left(newline).simplified();

    return preview;
}

QString DialogsModel::draftMessage(Dialog *dialog) const
{
    if(!dialog->draft())
        return QString();

    return dialog->draft()->message();
}

void DialogsModel::onSendStatusUpdated(Dialog *dialog)
{
    int idx = this->_dialogs.indexOf(dialog);

    if(idx == -1)
        return;

    Emit_DataChangedRoles(idx, DialogRoles::PeerActionRole);
}

void DialogsModel::onDialogUnreadCountChanged(Dialog *dialog)
{
    int idx = this->_dialogs.indexOf(dialog);

    if(idx == -1)
        return;

    Emit_DataChangedRoles(idx, DialogsModel::UnreadCountRole);
}

void DialogsModel::onDialogNotifySettingsChanged(Dialog *dialog)
{
    int idx = this->_dialogs.indexOf(dialog);

    if(idx == -1)
        return;

    Emit_DataChangedRoles(idx, DialogsModel::IsMutedRole);
}

void DialogsModel::onDialogNewMessage(Dialog *dialog)
{
    int idx = this->_dialogs.indexOf(dialog);

    if(idx == -1)
        return;

    Emit_DataChanged(idx);
    int newidx = this->insertionPoint(dialog);

    if (newidx == idx || newidx == (idx + 1)) {
        return;
    }

    if (!this->beginMoveRows(QModelIndex(), idx, idx, QModelIndex(), Safe_MoveIdx(idx, newidx))) {
        return;
    }
    this->_dialogs.move(idx, newidx);
    this->endMoveRows();
}

void DialogsModel::onDialogNewDraftMessage(Dialog *dialog)
{
    int idx = this->_dialogs.indexOf(dialog);

    if(idx == -1)
        return;

    DraftMessage* draftmessage = dialog->draft();

    if((idx > 0) && (draftmessage->constructorId() == TLTypes::DraftMessage))
    {
        this->beginMoveRows(QModelIndex(), idx, idx, QModelIndex(), 0);
        this->_dialogs.move(idx, 0);
        this->endMoveRows();
    }

    Emit_DataChanged(idx);
    int newidx = this->insertionPoint(dialog);

    if (newidx == idx || newidx == (idx + 1)) {
        return;
    }

    if (!this->beginMoveRows(QModelIndex(), idx, idx, QModelIndex(), Safe_MoveIdx(idx, newidx))) {
        return;
    }
    this->_dialogs.move(idx, newidx);
    this->endMoveRows();
}

void DialogsModel::onDialogDeleteMessage(Dialog *dialog)
{
    int idx = this->_dialogs.indexOf(dialog);

    if(idx == -1)
        return;

    Emit_DataChanged(idx);
    int newidx = this->insertionPoint(dialog, idx);

    if (newidx == idx || newidx == (idx + 1)) {
        return;
    }

    if (!this->beginMoveRows(QModelIndex(), idx, idx, QModelIndex(), Safe_MoveIdx(idx, newidx))) {
        return;
    }
    this->_dialogs.move(idx, newidx);
    this->endMoveRows();
}

void DialogsModel::onDialogDeleted(Dialog *dialog)
{
    int idx = this->_dialogs.indexOf(dialog);

    if(idx == -1)
        return;

    this->beginRemoveRows(QModelIndex(), idx, idx);
    this->_dialogs.removeAt(idx);
    this->endRemoveRows();
}

void DialogsModel::onDialogEditMessage(Dialog *dialog)
{
    int idx = this->_dialogs.indexOf(dialog);

    if(idx == -1)
        return;

    Emit_DataChangedRoles(idx, DialogsModel::TopMessageRole <<
                               DialogsModel::TopMessageDateRole <<
                               DialogsModel::TopMessageTextRole);
}

void DialogsModel::onNewDialogs(const TLVector<Dialog *> &newdialogs)
{
    foreach(Dialog* newdialog, newdialogs)
    {
        int idx = this->insertionPoint(newdialog);

        this->beginInsertRows(QModelIndex(), idx, idx);
        this->_dialogs.insert(idx, newdialog);
        this->endInsertRows();
    }
}

void DialogsModel::onReadHistory(Dialog *dialog)
{
    int idx = this->_dialogs.indexOf(dialog);

    if(idx == -1)
        return;

    Emit_DataChanged(idx);
}

void DialogsModel::onTitleChanged(Dialog *dialog)
{
    int idx = this->_dialogs.indexOf(dialog);

    if(idx == -1)
        return;

    Emit_DataChangedRoles(idx, DialogsModel::TitleRole);
}

void DialogsModel::telegramReady()
{
    this->beginResetModel();
    TelegramCache_loadDialogs(this->_dialogs);
    this->_telegram->sortDialogs(this->_dialogs);
    this->endResetModel();

    connect(TelegramCache_instance, &TelegramCache::newDialogs, this, &DialogsModel::onNewDialogs, Qt::UniqueConnection);
    connect(TelegramCache_instance, &TelegramCache::readHistory, this, &DialogsModel::onReadHistory, Qt::UniqueConnection);
    connect(TelegramCache_instance, &TelegramCache::titleChanged, this, &DialogsModel::onTitleChanged, Qt::UniqueConnection);
    connect(TelegramCache_instance, &TelegramCache::dialogUnreadCountChanged, this, &DialogsModel::onDialogUnreadCountChanged, Qt::UniqueConnection);
    connect(TelegramCache_instance, &TelegramCache::dialogNotifySettingsChanged, this, &DialogsModel::onDialogNotifySettingsChanged, Qt::UniqueConnection);
    connect(TelegramCache_instance, &TelegramCache::dialogNewMessage, this, &DialogsModel::onDialogNewMessage, Qt::UniqueConnection);
    connect(TelegramCache_instance, &TelegramCache::dialogNewDraftMessage, this, &DialogsModel::onDialogNewDraftMessage, Qt::UniqueConnection);
    connect(TelegramCache_instance, &TelegramCache::dialogDeleteMessage, this, &DialogsModel::onDialogDeleteMessage, Qt::UniqueConnection);
    connect(TelegramCache_instance, &TelegramCache::dialogEditMessage, this, &DialogsModel::onDialogEditMessage, Qt::UniqueConnection);
    connect(TelegramCache_instance, &TelegramCache::dialogDeleted, this, &DialogsModel::onDialogDeleted, Qt::UniqueConnection);

    connect(SendStatusHandler_instance, &SendStatusHandler::sendStatusUpdated, this, &DialogsModel::onSendStatusUpdated, Qt::UniqueConnection);

}
