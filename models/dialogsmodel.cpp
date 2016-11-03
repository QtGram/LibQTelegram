#include "dialogsmodel.h"
#include "../config/cache/telegramcache.h"

DialogsModel::DialogsModel(QObject *parent) : TelegramModel(parent)
{
    connect(TelegramCache_instance, &TelegramCache::newDialogs, this, &DialogsModel::onNewDialogs);
    connect(TelegramCache_instance, &TelegramCache::readHistory, this, &DialogsModel::onReadHistory);
    connect(TelegramCache_instance, &TelegramCache::dialogUnreadCountChanged, this, &DialogsModel::onDialogUnreadCountChanged);
    connect(TelegramCache_instance, &TelegramCache::dialogNewMessage, this, &DialogsModel::onDialogNewMessage);
    connect(TelegramCache_instance, &TelegramCache::dialogNewDraftMessage, this, &DialogsModel::onDialogNewDraftMessage);
    connect(TelegramCache_instance, &TelegramCache::dialogDeleteMessage, this, &DialogsModel::onDialogDeleteMessage);
    connect(TelegramCache_instance, &TelegramCache::dialogEditMessage, this, &DialogsModel::onDialogEditMessage);
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
        Message* message = TelegramCache_message(dialog->topMessage(), dialog);

        if(!message)
            return QVariant();

        return this->firstMessageLine(message);
    }

    if(role == DialogsModel::TopMessageDateRole)
    {
        Message* message = TelegramCache_message(dialog->topMessage(), dialog);

        if(!message)
            return QVariant();

        return TelegramHelper::dateString(message->date());
    }

    if(role == DialogsModel::IsTopMessageUnreadRole)
    {
        Message* message = TelegramCache_message(dialog->topMessage(), dialog);

        if(!message)
            return QVariant();

        return (dialog->readOutboxMaxId() < message->id());
    }

    if(role == DialogsModel::IsTopMessageOutRole)
    {
        Message* message = TelegramCache_message(dialog->topMessage(), dialog);

        if(!message)
            return QVariant();

        return message->isOut();
    }

    if(role == DialogsModel::IsTopMessageServiceRole)
    {
        Message* message = TelegramCache_message(dialog->topMessage(), dialog);

        if(!message)
            return QVariant();

        return message->constructorId() == TLTypes::MessageService;
    }

    if(role == DialogsModel::DraftMessageRole)
        return this->draftMessage(dialog);

    if(role == DialogsModel::IsChatRole)
        return TelegramHelper::isChat(dialog);

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

    return QVariant();
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

    return roles;
}

int DialogsModel::insertionPoint(Dialog *changeddialog, int fromidx) const
{
    if(!changeddialog->topMessage())
        return this->_dialogs.length() - 1;

    Message* msg1 = TelegramCache_message(changeddialog->topMessage(), changeddialog);

    for(int i = fromidx + 1; i < this->_dialogs.length(); i++)
    {
        Dialog* dialog = this->_dialogs[i];

        if(!dialog->topMessage())
            return i;

        Message* msg2 = TelegramCache_message(dialog->topMessage(), dialog);

        if(!msg2 || (msg1->date() > msg2->date()))
            return i;
    }

    return fromidx;
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
        return preview.left(newline - 1);

    return preview;
}

QString DialogsModel::draftMessage(Dialog *dialog) const
{
    if(!dialog->draft())
        return QString();

    return dialog->draft()->message();
}

void DialogsModel::sortDialogs()
{
    this->beginResetModel();

    std::sort(this->_dialogs.begin(), this->_dialogs.end(), [](Dialog* dlg1, Dialog* dlg2) {
        Message* msg1 = TelegramCache_message(dlg1->topMessage(), dlg1);
        Message* msg2 = TelegramCache_message(dlg2->topMessage(), dlg2);

        if(!msg1 || !msg2)
            return false;

        return msg1->date() > msg2->date();
    });

    this->endResetModel();
}

void DialogsModel::onDialogUnreadCountChanged(Dialog *dialog)
{
    int idx = this->_dialogs.indexOf(dialog);

    if(idx == -1)
        return;

    Emit_DataChangedRoles(idx, DialogsModel::UnreadCountRole);
}

void DialogsModel::onDialogNewMessage(Dialog *dialog)
{
    int idx = this->_dialogs.indexOf(dialog);

    if(idx == -1)
        return;

    Emit_DataChanged(idx);

    if(idx == 0)
        return;

    this->beginMoveRows(QModelIndex(), idx, idx, QModelIndex(), 0);
    this->_dialogs.move(idx, 0);
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
    int newidx = this->insertionPoint(dialog, idx);

    if(idx == newidx)
        return;

    this->beginMoveRows(QModelIndex(), idx, idx, QModelIndex(), newidx + 1);
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

    if(idx == newidx)
        return;

    this->beginMoveRows(QModelIndex(), idx, idx, QModelIndex(), newidx + 1);
    this->_dialogs.move(idx, newidx);
    this->endMoveRows();
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

void DialogsModel::onNewDialogs(const TLVector<Dialog *> &dialogs)
{
    this->_dialogs << dialogs;
    this->sortDialogs(); // NOTE: Remove this evil call
}

void DialogsModel::onReadHistory(Dialog *dialog)
{
    int idx = this->_dialogs.indexOf(dialog);

    if(idx == -1)
        return;

    Emit_DataChanged(idx);
}

void DialogsModel::telegramReady()
{
    this->_dialogs = TelegramCache_dialogs;
    this->sortDialogs();
}
