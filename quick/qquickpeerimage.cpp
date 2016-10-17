#include "qquickpeerimage.h"
#include "../config/cache/telegramcache.h"

QQuickPeerImage::QQuickPeerImage(QQuickItem* parent): QQuickBaseItem(parent), _peer(NULL), _size(0)
{
    this->_backcolor = qApp->palette().color(QPalette::Base);
    this->_forecolor = qApp->palette().color(QPalette::WindowText);
    this->_pixelsize = qApp->font().pointSize();
}

TelegramObject *QQuickPeerImage::peer() const
{
    return this->_peer;
}

int QQuickPeerImage::size() const
{
    return this->_size;
}

QColor QQuickPeerImage::backgroundColor() const
{
    return this->_backcolor;
}

QColor QQuickPeerImage::foregroundColor() const
{
    return this->_forecolor;
}

qreal QQuickPeerImage::fontPixelSize() const
{
    return this->_pixelsize;
}

void QQuickPeerImage::setPeer(TelegramObject *peer)
{
    if(this->_peer == peer)
        return;

    this->_peer = peer;
    this->initialize();
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

void QQuickPeerImage::setBackgroundColor(const QColor &color)
{
    if(this->_backcolor == color)
        return;

    this->_backcolor = color;
    emit backgroundColorChanged();
}

void QQuickPeerImage::setForegroundColor(const QColor &color)
{
    if(this->_forecolor == color)
        return;

    this->_forecolor = color;
    emit foregroundColorChanged();
}

void QQuickPeerImage::setFontPixelSize(qreal pixelsize)
{
    if(this->_pixelsize == pixelsize)
        return;

    this->_pixelsize = pixelsize;
    emit fontPixelSizeChanged();
}

void QQuickPeerImage::initialize()
{
    if(!this->_peer || this->_mediaelement)
        return;

    FileObject* fileobject = this->createFileObject(this->_peer);

    if(fileobject)
    {
        connect(fileobject, &FileObject::hasThumbnailChanged, this, &QQuickPeerImage::reinitialize);
        connect(fileobject, &FileObject::downloadedChanged, this, &QQuickPeerImage::reinitialize);
    }

    if(fileobject && (fileobject->hasThumbnail() || fileobject->downloaded()))
    {
        this->createRoundImageElement();
        this->bindToElement();
    }
    else
        this->createPlaceHolderImageElement();
}

void QQuickPeerImage::updateMetrics()
{
    this->setWidth(this->_size);
    this->setHeight(this->_size);
}

void QQuickPeerImage::createRoundImageElement()
{
    QString componentsource = "Item {\n"
                                  "property url source\n"
                                  "id: roundimage\n"
                                  "anchors.fill: parent\n"
                                  "Image {\n"
                                      "id: image\n"
                                      "anchors.fill: parent\n"
                                      "asynchronous: true\n"
                                      "fillMode: Image.PreserveAspectFit\n"
                                      "visible: false\n"
                                      "source: roundimage.source\n"
                                  "}\n"
                                  "Rectangle {\n"
                                      "id: mask\n"
                                      "anchors.fill: parent\n"
                                      "radius: width * 0.5\n"
                                      "visible: false\n"
                                      "color: \"black\"\n"
                                  "}\n"
                                  "OpacityMask {\n"
                                      "anchors.fill: mask\n"
                                      "source: image\n"
                                      "maskSource: mask\n"
                                  "}"
                              "}";

    this->_mediaelement = this->createComponent(componentsource);
}

void QQuickPeerImage::createPlaceHolderImageElement()
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

    this->createComponent(componentsource.arg(this->_backcolor.name())
                                         .arg(this->_pixelsize)
                                         .arg(this->_forecolor.name())
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
