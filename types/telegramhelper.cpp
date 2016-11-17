#include "telegramhelper.h"
#include "../crypto/hash.h"
#include "../config/telegramconfig.h"
#include <QDateTime>

TelegramHelper::TelegramHelper()
{

}

Message *TelegramHelper::createMessage(Updates *updates, User* me)
{
    Q_ASSERT((updates->constructorId() == TLTypes::UpdateShortMessage) ||
             (updates->constructorId() == TLTypes::UpdateShortChatMessage));

    Message* message = new Message();
    message->setConstructorId(TLTypes::Message);
    message->setFlags(updates->flags());
    message->setIsOut(updates->isOut());
    message->setIsMentioned(updates->isMentioned());
    message->setIsMediaUnread(updates->isMediaUnread());
    message->setIsSilent(updates->isSilent());
    message->setId(updates->id());
    message->setMessage(updates->message());
    message->setDate(updates->date());
    message->setFwdFrom(updates->fwdFrom());
    message->setViaBotId(updates->viaBotId());
    message->setReplyToMsgId(updates->replyToMsgId());
    message->setEntities(updates->entities());

    if(updates->constructorId() == TLTypes::UpdateShortMessage)
    {
        Peer* topeer = new Peer();
        topeer->setConstructorId(TLTypes::PeerUser);
        topeer->setUserId(updates->isOut() ? updates->userId() : me->id());

        message->setFromId(updates->isOut() ? me->id() : updates->userId());
        message->setToId(topeer);
    }
    else if(updates->constructorId() == TLTypes::UpdateShortChatMessage)
    {
        Peer* topeer = new Peer();
        topeer->setConstructorId(TLTypes::PeerChat);
        topeer->setChatId(updates->chatId());

        message->setFromId(updates->fromId());
        message->setToId(topeer);
    }

    return message;
}

Message *TelegramHelper::createMessage(const QString &text, User* me, Peer* peer)
{
    Message* message = new Message();
    message->setConstructorId(TLTypes::Message);
    message->setMessage(text);
    message->setDate(QDateTime::currentDateTime().toTime_t());
    message->setFromId(me->id());
    message->setIsOut(true);
    message->setToId(peer->clone<Peer>());

    return message;
}

Dialog *TelegramHelper::createDialog(User *user, QObject *parent)
{
    Peer* peer = new Peer();
    peer->setConstructorId(TLTypes::PeerUser);
    peer->setUserId(user->id());

    Dialog* dialog = new Dialog(parent);
    dialog->setConstructorId(TLTypes::Dialog);
    dialog->setPeer(peer);

    return dialog;
}

InputFileLocation *TelegramHelper::inputFileLocation(FileLocation *filelocation)
{
    InputFileLocation* inputfilelocation = new InputFileLocation();

    inputfilelocation->setConstructorId(TLTypes::InputFileLocation);
    inputfilelocation->setVolumeId(filelocation->volumeId());
    inputfilelocation->setLocalId(filelocation->localId());
    inputfilelocation->setSecret(filelocation->secret());

    return inputfilelocation;
}

InputFileLocation *TelegramHelper::inputFileLocation(Document *document)
{
    InputFileLocation* inputfilelocation = new InputFileLocation();

    inputfilelocation->setConstructorId(TLTypes::InputDocumentFileLocation);
    inputfilelocation->setId(document->id());
    inputfilelocation->setAccessHash(document->accessHash());
    inputfilelocation->setVersion(document->version());

    return inputfilelocation;
}

InputNotifyPeer *TelegramHelper::inputNotifyPeer(Dialog *dialog, TLLong accesshash, QObject *parent)
{
    InputPeer* inputpeer = TelegramHelper::inputPeer(dialog, accesshash);
    InputNotifyPeer* inputnotifypeer = new InputNotifyPeer(parent);

    inputnotifypeer->setConstructorId(TLTypes::InputNotifyPeer);
    inputnotifypeer->setPeer(inputpeer);
    return inputnotifypeer;
}

InputPeerNotifySettings *TelegramHelper::inputPeerNotifySettings(PeerNotifySettings *notifysettings, QObject *parent)
{
    InputPeerNotifySettings* inputpeernotifysettings = new InputPeerNotifySettings(parent);

    inputpeernotifysettings->setConstructorId(TLTypes::InputPeerNotifySettings);
    inputpeernotifysettings->setIsShowPreviews(!notifysettings->isSilent());
    inputpeernotifysettings->setIsSilent(notifysettings->isSilent());
    inputpeernotifysettings->setMuteUntil(notifysettings->muteUntil());
    return inputpeernotifysettings;
}

InputPeer *TelegramHelper::inputPeer(Dialog *dialog, TLLong accesshash, QObject *parent)
{
    return TelegramHelper::inputPeer(dialog->peer(), accesshash, parent);
}

InputPeer *TelegramHelper::inputPeer(Peer *peer, TLLong accesshash, QObject *parent)
{
    InputPeer* inputpeer = new InputPeer(parent);
    inputpeer->setAccessHash(accesshash);

    if(peer->constructorId() == TLTypes::PeerUser)
    {
        inputpeer->setConstructorId(TLTypes::InputPeerUser);
        inputpeer->setUserId(peer->userId());
    }
    else if(peer->constructorId() == TLTypes::PeerChat)
    {
        inputpeer->setConstructorId(TLTypes::InputPeerChat);
        inputpeer->setChatId(peer->chatId());
    }
    else if(peer->constructorId() == TLTypes::PeerChannel)
    {
        inputpeer->setConstructorId(TLTypes::InputPeerChannel);
        inputpeer->setChannelId(peer->channelId());
    }
    else
        Q_ASSERT(false);

    return inputpeer;
}

InputPeer *TelegramHelper::inputPeer(Message *message, QObject *parent)
{
    InputPeer* inputpeer = new InputPeer(parent);
    TLInt dialogid = TelegramHelper::messageToDialog(message);

    if(TelegramHelper::isChannel(message->toId()))
    {
        inputpeer->setConstructorId(TLTypes::InputPeerChannel);
        inputpeer->setChannelId(dialogid);
    }
    else if(TelegramHelper::isChat(message->toId()))
    {
        inputpeer->setConstructorId(TLTypes::InputPeerChat);
        inputpeer->setChatId(dialogid);
    }
    else
    {
        inputpeer->setConstructorId(TLTypes::InputPeerUser);
        inputpeer->setUserId(dialogid);
    }

    return inputpeer;
}

InputChannel *TelegramHelper::inputChannel(Dialog *dialog, TLLong accesshash, QObject *parent)
{
    if(!TelegramHelper::isChannel(dialog))
        return NULL;

    InputChannel* inputchannel = new InputChannel(parent);
    inputchannel->setConstructorId(TLTypes::InputChannel);
    inputchannel->setChannelId(TelegramHelper::identifier(dialog));
    inputchannel->setAccessHash(accesshash);
    return inputchannel;
}

InputChannel *TelegramHelper::inputChannel(Chat *chat, QObject *parent)
{
    if(!TelegramHelper::isChannel(chat))
        return NULL;

    InputChannel* inputchannel = new InputChannel(parent);
    inputchannel->setConstructorId(TLTypes::InputChannel);
    inputchannel->setChannelId(chat->id());
    inputchannel->setAccessHash(chat->accessHash());
    return inputchannel;
}

InputMedia *TelegramHelper::inputMediaFile(FileUploader *fileuploader, QObject *parent)
{
    InputMedia* inputmedia = new InputMedia(parent);
    inputmedia->setConstructorId(TLTypes::InputMediaUploadedDocument);
    inputmedia->setFile(TelegramHelper::inputFile(fileuploader));
    inputmedia->setMimeType(ToTLString(fileuploader->mimeType()));
    inputmedia->setCaption(fileuploader->caption());

    TLVector<DocumentAttribute*> attributes;
    attributes << TelegramHelper::createDocumentAttribute(fileuploader->fileName(), inputmedia);

    inputmedia->setAttributes(attributes);
    return inputmedia;
}

InputMedia *TelegramHelper::inputMediaPhoto(FileUploader *fileuploader, QObject *parent)
{
    InputMedia* inputmedia = new InputMedia(parent);
    inputmedia->setConstructorId(TLTypes::InputMediaUploadedPhoto);
    inputmedia->setCaption(ToTLString(fileuploader->caption()));
    inputmedia->setFile(TelegramHelper::inputFile(fileuploader));
    return inputmedia;
}

InputMedia *TelegramHelper::inputMediaGeoPoint(TLDouble latitude, TLDouble longitude, QObject *parent)
{
    InputGeoPoint* inputgeopoint = new InputGeoPoint();
    inputgeopoint->setConstructorId(TLTypes::InputGeoPoint);
    inputgeopoint->setLatitude(latitude);
    inputgeopoint->setLongitude(longitude);

    InputMedia* inputmedia = new InputMedia(parent);
    inputmedia->setConstructorId(TLTypes::InputMediaGeoPoint);
    inputmedia->setGeoPoint(inputgeopoint);
    return inputmedia;
}

InputFile *TelegramHelper::inputFile(FileUploader *fileuploader, QObject *parent)
{
    InputFile* inputfile = new InputFile(parent);
    inputfile->setId(fileuploader->fileId());
    inputfile->setParts(fileuploader->partsCount());
    inputfile->setName(ToTLString(fileuploader->fileName()));

    if(!fileuploader->isBigFile())
    {
        inputfile->setConstructorId(TLTypes::InputFile);
        inputfile->setMd5Checksum(ToTLString(fileuploader->md5hash()));
    }
    else
        inputfile->setConstructorId(TLTypes::InputFileBig);

    return inputfile;
}

InputUser *TelegramHelper::inputUser(User *user, QObject *parent)
{
    InputUser* inputuser = new InputUser(parent);
    inputuser->setConstructorId(TLTypes::InputUser);
    inputuser->setUserId(user->id());
    inputuser->setAccessHash(user->accessHash());
    return inputuser;
}

PhotoSize *TelegramHelper::photoSmall(Photo *photo)
{
    return photo->sizes().first();
}

PhotoSize *TelegramHelper::photoBig(Photo *photo)
{
    return photo->sizes().last();
}

ChatPhoto *TelegramHelper::chatPhoto(Photo *photo, QObject* parent)
{
    ChatPhoto* chatphoto = new ChatPhoto(parent);

    if(photo->constructorId() != TLTypes::PhotoEmpty)
    {
        PhotoSize* smallphoto = TelegramHelper::photoSmall(photo);
        PhotoSize* bigphoto = TelegramHelper::photoBig(photo);

        chatphoto->setConstructorId(TLTypes::ChatPhoto);
        chatphoto->setPhotoSmall(smallphoto->location()->clone<FileLocation>());
        chatphoto->setPhotoBig(bigphoto->location()->clone<FileLocation>());
    }
    else
        chatphoto->setConstructorId(TLTypes::ChatPhotoEmpty);

    return chatphoto;
}

QString TelegramHelper::duration(TLInt seconds)
{
    QDateTime datetime = QDateTime::fromTime_t(seconds);
    QTime time = datetime.time();

    if(time.hour() > 1)
        return datetime.toUTC().toString("HH:mm:ss");

    return datetime.toUTC().toString("mm:ss");
}

QString TelegramHelper::dateString(TLInt timestamp)
{
    QDateTime datetime = QDateTime::fromTime_t(timestamp);
    const QDateTime& currentdatetime = QDateTime::currentDateTime();
    qint64 seconds = datetime.secsTo(currentdatetime);
    int days = datetime.daysTo(currentdatetime);

    if(seconds < 24 * 60 * 60)
        return days ? QObject::tr("Yesterday %1").arg(datetime.toString("HH:mm")) : datetime.toString("HH:mm");

    return datetime.toString("MMM dd, HH:mm");
}

QString TelegramHelper::fileSize(TLDouble size)
{
    int unit = 0;

    while(size > 1024)
    {
        size /= 1024;
        unit++;
    }

    QString filesize = QString::number(size, 'g', 2);

    if(unit == 0)
        filesize += "B";
    else if(unit == 1)
        filesize += "KB";
    else if(unit == 2)
        filesize += "MB";
    else if(unit == 3)
        filesize += "GB";

    return filesize;
}

QString TelegramHelper::fullName(User *user)
{
    if(!user)
        return QString();

    if(user->lastName().isEmpty())
        return user->firstName();

    return user->firstName() + " " + user->lastName();
}

QString TelegramHelper::statusText(User *user)
{
    if(TelegramConfig_me->id() == user->id())
        return QObject::tr("Chat with yourself");

    if(user->status())
    {
        if(user->status()->constructorId() == TLTypes::UserStatusLastMonth)
            return QObject::tr("Last month");

        if(user->status()->constructorId() == TLTypes::UserStatusLastWeek)
            return QObject::tr("Last week");

        if(user->status()->constructorId() == TLTypes::UserStatusOffline)
            return QObject::tr("Last seen %1").arg(TelegramHelper::dateString(user->status()->wasOnline()));

        if(user->status()->constructorId() == TLTypes::UserStatusOnline)
        {
            if(CurrentTimeStamp >= static_cast<uint>(user->status()->expires()))
                return QObject::tr("Last seen %1").arg(TelegramHelper::dateString(user->status()->expires()));

            return QObject::tr("Online");
        }

        if(user->status()->constructorId() == TLTypes::UserStatusRecently)
            return QObject::tr("Recently");
    }

    return QObject::tr("Long time ago");
}

DocumentAttribute *TelegramHelper::createDocumentAttribute(const QString &filename, QObject* parent)
{
    DocumentAttribute* documentattribute = new DocumentAttribute(parent);
    documentattribute->setConstructorId(TLTypes::DocumentAttributeFilename);
    documentattribute->setFileName(filename);
    return documentattribute;
}

DocumentAttribute *TelegramHelper::documentHas(Document *document, TLConstructor attributector)
{
    foreach(DocumentAttribute* attribute, document->attributes())
    {
        if(attribute->constructorId() == attributector)
            return attribute;
    }

    return NULL;
}

bool TelegramHelper::isSticker(Document *document)
{
    return TelegramHelper::documentHas(document, TLTypes::DocumentAttributeSticker) != NULL;
}

bool TelegramHelper::isAnimated(Document *document)
{
    return TelegramHelper::documentHas(document, TLTypes::DocumentAttributeAnimated) != NULL;
}

bool TelegramHelper::isVideo(Document *document)
{
    return TelegramHelper::documentHas(document, TLTypes::DocumentAttributeVideo) != NULL;
}

bool TelegramHelper::isAudio(Document *document)
{
    return TelegramHelper::documentHas(document, TLTypes::DocumentAttributeAudio) != NULL &&
           TelegramHelper::documentHas(document, TLTypes::DocumentAttributeFilename) == NULL;
}

bool TelegramHelper::isFile(Document *document)
{
    return TelegramHelper::documentHas(document, TLTypes::DocumentAttributeFilename) != NULL;
}

bool TelegramHelper::isMuted(Dialog *dialog)
{
    PeerNotifySettings* notifysettings = dialog->notifySettings();

    if(!notifysettings || (notifysettings->constructorId() == TLTypes::PeerNotifySettingsEmpty))
        return false;

    return notifysettings->isSilent() || (CurrentTimeStamp <= static_cast<uint>(notifysettings->muteUntil()));
}

bool TelegramHelper::isCloud(Dialog *dialog)
{
    return TelegramHelper::identifier(dialog) == TelegramConfig_me->id();
}

bool TelegramHelper::isChat(Dialog *dialog)
{
    return TelegramHelper::isChat(dialog->peer());
}

bool TelegramHelper::isChannel(Dialog *dialog)
{
    return TelegramHelper::isChannel(dialog->peer());
}

bool TelegramHelper::isChat(Peer *peer)
{
    return peer->constructorId() == TLTypes::PeerChat;
}

bool TelegramHelper::isChannel(Peer *peer)
{
    return peer->constructorId() == TLTypes::PeerChannel;
}

bool TelegramHelper::isChannel(Chat *chat)
{
    return (chat->constructorId() == TLTypes::Channel) || (chat->constructorId() == TLTypes::ChannelForbidden);
}

bool TelegramHelper::messageIsWebPagePending(Message *message)
{
    if(!message || !message->media() || (message->media()->constructorId() != TLTypes::MessageMediaWebPage))
        return false;

    return message->media()->webpage()->constructorId() == TLTypes::WebPagePending;
}

bool TelegramHelper::messageIsAction(Message *message)
{
    if(!message || !message->action() || (message->action()->constructorId() == TLTypes::MessageService))
        return false;

    return true;
}

MessageId TelegramHelper::identifier(TLInt messageid, TLInt channelid)
{
    return (static_cast<MessageId>(channelid) << 32u) + messageid;
}

MessageId TelegramHelper::identifier(TLInt messageid, Dialog *dialog)
{
    if(TelegramHelper::isChannel(dialog))
        return TelegramHelper::identifier(messageid, TelegramHelper::identifier(dialog));

    return messageid;
}

MessageId TelegramHelper::identifier(Message *message)
{
    if(TelegramHelper::isChannel(message->toId()))
        return TelegramHelper::identifier(message->id(), TelegramHelper::messageToDialog(message));

    return message->id();
}

TLInt TelegramHelper::identifier(User *user)
{
    return user->id();
}

TLInt TelegramHelper::identifier(Chat *chat)
{
    return chat->id();
}

TLInt TelegramHelper::identifier(Peer *peer)
{
    if(peer->constructorId() == TLTypes::PeerChannel)
        return peer->channelId();

    if(peer->constructorId() == TLTypes::PeerChat)
        return peer->chatId();

    if(peer->constructorId() == TLTypes::PeerUser)
        return peer->userId();

    Q_ASSERT(false);
    return -1;
}

TLInt TelegramHelper::identifier(Dialog *dialog)
{
    return TelegramHelper::identifier(dialog->peer());
}

TLInt TelegramHelper::messageToDialog(Message *message)
{
    if(message->isOut() || TelegramHelper::isChat(message->toId()) || TelegramHelper::isChannel(message->toId()))
        return TelegramHelper::identifier(message->toId());

    return message->fromId();
}

TLInt TelegramHelper::messageIdentifier(MessageId messageid)
{
    return messageid & 0xFFFFFFFF;
}

TLBytes TelegramHelper::createPasswordHash(const QString &password, const AccountPassword *accountpassword)
{
    TLBytes newpassworddata = password.isEmpty() ? TLBytes() : (accountpassword->currentSalt() + password.toUtf8() + accountpassword->currentSalt());
    return sha256_hash(newpassworddata);
}
