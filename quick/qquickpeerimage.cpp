#include "qquickpeerimage.h"
#include "../config/cache/telegramcache.h"

QQuickPeerImage::QQuickPeerImage(QQuickItem* parent): QQuickBaseItem(parent), _peer(NULL), _size(0)
{
}

TelegramObject *QQuickPeerImage::peer() const
{
    return this->_peer;
}

int QQuickPeerImage::size() const
{
    return this->_size;
}

void QQuickPeerImage::setPeer(TelegramObject *peer)
{
    if(this->_peer == peer)
        return;

    this->_peer = peer;
    emit peerChanged();
}

void QQuickPeerImage::setSize(int size)
{
    if(this->_size == size)
        return;

    this->_size = size;
    this->updateMetrics();
    emit sizeChanged();
}

TelegramObject *QQuickPeerImage::findPeer(TelegramObject *peer)
{
    if(peer->constructorId() != TLTypes::Message)
        return peer;

    Message* message = qobject_cast<Message*>(peer);

    if(message->isPost() || !message->fromId())
        return peer;

    User* user = TelegramCache_user(message->fromId());

    if(!user)
        return peer;

    return user;
}

void QQuickPeerImage::initialize()
{
    if(!this->_peer || this->_mediaelement)
        return;

    FileObject* fileobject = this->createFileObject(this->findPeer(this->_peer));

    if(fileobject)
    {
        connect(fileobject, &FileObject::hasThumbnailChanged, this, &QQuickPeerImage::reinitialize);
        connect(fileobject, &FileObject::downloadedChanged, this, &QQuickPeerImage::reinitialize);
        connect(fileobject, &FileObject::thumbnailChanged, this, &QQuickBaseItem::sourceChanged);
        connect(fileobject, &FileObject::filePathChanged, this, &QQuickBaseItem::sourceChanged);
    }

    if(fileobject && (fileobject->hasThumbnail() || fileobject->downloaded()))
    {
        this->createPeerElement();
        this->bindToElement();
    }
    else
        this->createFallbackPeerElement();
}

void QQuickPeerImage::updateMetrics()
{
    this->setWidth(this->_size);
    this->setHeight(this->_size);
}

void QQuickPeerImage::updateSource(QVariant sourcevalue)
{
    this->_mediaelement->setProperty("source", sourcevalue);
}

void QQuickPeerImage::createPeerElement()
{
    QString componentsource = "Item {\n"
                                  "property url source\n"
                                  "id: roundimage\n"
                                  "anchors.fill: parent\n"
                                  "layer.enabled: true\n"
                                  "layer.effect: OpacityMask {\n"
                                      "maskSource: Rectangle {\n"
                                          "width: roundimage.width\n"
                                          "height: roundimage.height\n"
                                          "radius: width * 0.5\n"
                                      "}\n"
                                  "}\n"
                                  "Image {\n"
                                      "id: image\n"
                                      "anchors.fill: parent\n"
                                      "asynchronous: true\n"
                                      "fillMode: Image.PreserveAspectFit\n"
                                      "source: roundimage.source\n"
                                  "}\n"
                              "}";

    this->createComponent(componentsource);
}

void QQuickPeerImage::createFallbackPeerElement()
{
    QString componentsource = "Item {\n"
                                  "anchors.fill: parent\n"
                                  "Rectangle {\n"
                                      "anchors.fill: parent\n"
                                      "radius: width * 0.5\n"
                                      "color: \"%1\"\n"
                                  "}\n"
                                  "Text {\n"
                                      "anchors.centerIn: parent\n"
                                      "font.pixelSize: %2\n"
                                      "color: \"%3\"\n"
                                      "text: \"%4\"\n"
                                  "}\n"
                              "}";

    this->createComponent(componentsource.arg(this->backgroundColor().name())
                                         .arg(this->fontPixelSize())
                                         .arg(this->foregroundColor().name())
                                         .arg(this->fallbackText(this->_peer)));
}

QString QQuickPeerImage::fallbackText(TelegramObject *telegramobj)
{
    if(!telegramobj)
        return QString();

    if(telegramobj->constructorId() == TLTypes::Dialog)
        return this->fallbackText(qobject_cast<Dialog*>(telegramobj)->peer());

    if(telegramobj->constructorId() == TLTypes::Chat)
        return this->fallbackTitle(qobject_cast<Chat*>(telegramobj)->title());

    if(telegramobj->constructorId() == TLTypes::User)
        return this->fallbackTitle(TelegramHelper::fullName(qobject_cast<User*>(telegramobj)));

    if(telegramobj->constructorId() == TLTypes::PeerChannel)
    {
        Chat* chat = TelegramCache_chat(qobject_cast<Peer*>(telegramobj)->channelId());
        return this->fallbackText(chat);
    }

    if(telegramobj->constructorId() == TLTypes::PeerChat)
    {
        Chat* chat = TelegramCache_chat(qobject_cast<Peer*>(telegramobj)->chatId());
        return this->fallbackText(chat);
    }

    if(telegramobj->constructorId() == TLTypes::PeerUser)
    {
        User* user = TelegramCache_user(qobject_cast<Peer*>(telegramobj)->userId());
        return this->fallbackText(user);
    }

    return QString();
}

QString QQuickPeerImage::fallbackTitle(const QString &title)
{
    QStringList words = title.split(" ");

    if(words.length() >= 2)
        return words[0].mid(0, 1).toUpper() + words[1].mid(0, 1).toUpper();

    return words[0].mid(0, 1).toUpper();
}

void QQuickPeerImage::reinitialize()
{
    QQuickItem* olditem = this->_mediaelement;
    this->_mediaelement = NULL;

    this->initialize();
    olditem->deleteLater();
}

void QQuickPeerImage::bindToElement()
{
    if(!this->_mediaelement)
        return;

    QUrl mediaurl;

    if(this->downloaded())
        mediaurl = QUrl::fromLocalFile(this->filePath());
    else if(this->hasThumbnail())
        mediaurl = QUrl::fromLocalFile(this->thumbnail());
    else
        return;

    this->updateSource(mediaurl);
}

void QQuickPeerImage::componentComplete()
{
    QQuickBaseItem::componentComplete();

    if(this->_peer)
        this->initialize();
}
