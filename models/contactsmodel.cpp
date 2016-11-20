#include "contactsmodel.h"
#include "cache/telegramcache.h"

ContactsModel::ContactsModel(QObject *parent) : TelegramModel(parent), _pendingdialogid(0)
{
}

void ContactsModel::createDialog(User *user)
{
    Dialog* dialog = TelegramCache_instance->dialog(user->id(), true);

    if(!dialog)
    {
        dialog = TelegramHelper::createDialog(user);
        TelegramCache_insert(dialog);
    }

    emit dialogCreated(dialog);
}

void ContactsModel::createChat(const QString& title, const QVariantList& users)
{
    TLVector<InputUser*> inputusers;
    inputusers << TelegramHelper::inputUser(TelegramConfig_me, this);

    foreach(QVariant user, users)
        inputusers << TelegramHelper::inputUser(user.value<User*>(), this);

    MTProtoRequest* req = TelegramAPI::messagesCreateChat(DC_MainSession, inputusers, ToTLString(title));
    connect(req, &MTProtoRequest::replied, this, &ContactsModel::onCreateChannelOrChatReplied);
    qDeleteAll(inputusers);
}

void ContactsModel::createChannel(const QString &title, const QString& description)
{
    MTProtoRequest* req = TelegramAPI::channelsCreateChannel(DC_MainSession, ToTLString(title), ToTLString(description));
    connect(req, &MTProtoRequest::replied, this, &ContactsModel::onCreateChannelOrChatReplied);
}

QVariant ContactsModel::data(const QModelIndex &index, int role) const
{
    if(role == ContactsModel::ItemRole)
        return QVariant::fromValue(this->_contacts[index.row()]);

    if(role == ContactsModel::FirstLetterRole)
    {
        User* user = this->_contacts[index.row()];
        return TelegramHelper::fullName(user).at(0).toUpper();
    }

    if(role == ContactsModel::FullNameRole)
    {
        User* user = this->_contacts[index.row()];
        return TelegramHelper::fullName(user);
    }

    if(role == ContactsModel::StatusText)
    {
        User* user = this->_contacts[index.row()];
        return TelegramHelper::statusText(user);
    }

    return QVariant();
}

int ContactsModel::rowCount(const QModelIndex &) const
{
    return this->_contacts.length();
}

QHash<int, QByteArray> ContactsModel::roleNames() const
{
    QHash<int, QByteArray> roles = this->initRoles();

    roles[ContactsModel::FirstLetterRole] = "firstLetter";
    roles[ContactsModel::FullNameRole] = "fullName";
    roles[ContactsModel::StatusText] = "statusText";

    return roles;
}

void ContactsModel::telegramReady()
{
    connect(TelegramCache_instance, &TelegramCache::newDialogs, this, &ContactsModel::onNewDialogs);
    connect(TelegramCache_instance, &TelegramCache::contactsUpdated, this, &ContactsModel::loadContacts);

    this->loadContacts();
}

void ContactsModel::onNewDialogs(const TLVector<Dialog *> &dialogs)
{
    if(!this->_pendingdialogid)
        return;

    foreach(Dialog* dialog, dialogs)
    {
        if(TelegramHelper::identifier(dialog) == this->_pendingdialogid)
        {
            this->_pendingdialogid = 0;
            emit dialogCreated(dialog);
            return;
        }
    }
}

void ContactsModel::onCreateChannelOrChatReplied(MTProtoReply *mtreply)
{
    Updates updates;
    updates.read(mtreply);

    TLInt dialogid = this->getDialogId(&updates);

    if(!dialogid)
        return;

    this->_pendingdialogid = dialogid;
    Dialog* dialog = TelegramCache_instance->dialog(dialogid, true);

    if(!dialog)
        return;

    this->_pendingdialogid = 0;
    emit dialogCreated(dialog);
}

void ContactsModel::loadContacts()
{
    this->_contacts = TelegramCache_contacts;

    if(this->_contacts.isEmpty())
        return;

    this->beginResetModel();

    std::sort(this->_contacts.begin(), this->_contacts.end(), [](User* usr1, User* usr2) {
        return TelegramHelper::fullName(usr1) < TelegramHelper::fullName(usr2);
    });

    this->endResetModel();
}

TLInt ContactsModel::getDialogId(Updates *updates) const
{
    foreach(Update* update, updates->updates())
    {
        if(update->constructorId() == TLTypes::UpdateNewMessage)
            return TelegramHelper::messageToDialog(update->messageUpdatenewmessage());
    }

    qWarning("Cannot find the first message of the new chat");
    return 0;
}
