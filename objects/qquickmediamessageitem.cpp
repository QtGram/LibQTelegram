#include "qquickmediamessageitem.h"
#include <QDateTime>

QQuickMediaMessageItem::QQuickMediaMessageItem(QQuickItem *parent): QQuickItem(parent), _mediaelement(NULL), _fileobject(NULL), _telegram(NULL), _message(NULL), _size(0)
{
    this->_version = "2.0"; // Target base version by default
}

QString QQuickMediaMessageItem::version() const
{
    return this->_version;
}

Telegram *QQuickMediaMessageItem::telegram() const
{
    return this->_telegram;
}

Message *QQuickMediaMessageItem::message() const
{
    return this->_message;
}

int QQuickMediaMessageItem::size() const
{
    return this->_size;
}

QSize QQuickMediaMessageItem::imageSize() const
{
    if(!this->_fileobject)
        return QSize();

    return this->_fileobject->imageSize();
}

void QQuickMediaMessageItem::setVersion(const QString &version)
{
    if(this->_version == version)
        return;

    this->_version = version;
    emit versionChanged();
}

void QQuickMediaMessageItem::setTelegram(Telegram *telegram)
{
    if(this->_telegram == telegram)
        return;

    this->_telegram = telegram;
    this->initialize();
    emit telegramChanged();
}

void QQuickMediaMessageItem::setSize(int size)
{
    if(this->_size == size)
        return;

    this->_size = size;
    this->updateMetrics();
    emit sizeChanged();
}

void QQuickMediaMessageItem::setMessage(Message *message)
{
    if(this->_message == message)
        return;

    this->_message = message;
    this->initialize();
    emit messageChanged();
}

void QQuickMediaMessageItem::download()
{
    if(!this->_fileobject)
        return;

    this->_fileobject->download();
}

QQuickItem* QQuickMediaMessageItem::createComponent(const QString &componentcode)
{
    QQmlEngine *engine = qmlEngine(this);
    QQmlContext *context = qmlContext(this);

    if(!engine || !context)
    {
        qFatal("Cannot get engine or context");
        return NULL;
    }

    QQmlComponent component(engine);

    component.setData(QString("import QtQuick %1\n"
                              "import QtMultimedia 5.0\n"
                              "import QtGraphicalEffects 1.0\n"
                              "%2").arg(this->_version, componentcode).toUtf8(), QUrl());

    QQuickItem* item = qobject_cast<QQuickItem*>(component.create(context));

    if(!item)
    {
        qWarning() << component.errorString();
        return NULL;
    }

    item->setParent(this);
    item->setParentItem(this);
    return item;
}

bool QQuickMediaMessageItem::documentIsSticker(Document *document)
{
    foreach(DocumentAttribute* attribute, document->attributes())
    {
        if(attribute->constructorId() == TLTypes::DocumentAttributeSticker)
            return true;
    }

    return false;
}

bool QQuickMediaMessageItem::documentIsAnimated(Document *document)
{
    foreach(DocumentAttribute* attribute, document->attributes())
    {
        if(attribute->constructorId() == TLTypes::DocumentAttributeAnimated)
            return true;
    }

    return false;
}

void QQuickMediaMessageItem::initialize()
{
    if(!this->_telegram || !this->_message || this->_mediaelement)
        return;

    MessageMedia* messagemedia = this->_message->media();

    if(!messagemedia || (messagemedia->constructorId() == TLTypes::MessageMediaEmpty))
    {
        this->setVisible(false);
        return;
    }

    if(messagemedia->constructorId() == TLTypes::MessageMediaPhoto)
        this->createImageElement();
    else if(messagemedia->constructorId() == TLTypes::MessageMediaDocument)
    {
        Document* document = messagemedia->document();

        if(this->documentIsSticker(document))
            this->createImageElement();
        else if(this->documentIsAnimated(document))
            this->createAnimatedElement();
    }

    this->_fileobject = this->_telegram->fileObject(this->_message);

    connect(this->_fileobject, &FileObject::imageSizeChanged, this, &QQuickMediaMessageItem::imageSizeChanged);
    connect(this->_fileobject, &FileObject::imageSizeChanged, this, &QQuickMediaMessageItem::updateMetrics);
    connect(this->_fileobject, &FileObject::thumbnailChanged, [this]() { this->applySource(); });
    connect(this->_fileobject, &FileObject::filePathChanged, [this]() { this->applySource(); });

    this->updateMetrics();
    this->applySource();
}

void QQuickMediaMessageItem::createImageElement()
{
    this->_mediaelement = this->createComponent("Image {\n"
                                                    "anchors.fill: parent\n"
                                                    "asynchronous: true\n"
                                                "}");
}

void QQuickMediaMessageItem::createAnimatedElement()
{
    QString componentsource = "Item {\n"
                                  "property string source\n"
                                  "id: animatedelement\n"
                                  "anchors.fill: parent\n"
                                  "MediaPlayer {\n"
                                      "id: mediaplayer\n"
                                      "loops: MediaPlayer.Infinite\n"
                                      "autoPlay: true\n"
                                      "source: animatedelement.source\n"
                                  "}\n"
                                  "VideoOutput {\n"
                                      "id: videooutput\n"
                                      "anchors.fill: parent\n"
                                      "source: mediaplayer\n"
                                  "}\n"
                              "}";

    this->_mediaelement = this->createComponent(componentsource);
}

void QQuickMediaMessageItem::applySource()
{
    QString mediapath = "file://";

    if(this->_fileobject->downloaded())
        mediapath += this->_fileobject->filePath();
    else if(this->_fileobject->hasThumbnail())
        mediapath += this->_fileobject->thumbnail();
    else
        return;

    this->_mediaelement->setProperty("source", mediapath);
}

void QQuickMediaMessageItem::updateMetrics()
{
    if(!this->_size || !this->_fileobject)
        return;

    QSize imagesize = this->_fileobject->imageSize();

    if((imagesize.width() <= 0) || (imagesize.height() <= 0))
        return;

    qreal aspectratio = imagesize.width() / static_cast<qreal>(imagesize.height());

    this->setWidth(this->_size);
    this->setHeight(aspectratio ? (this->_size / aspectratio) : 0);
}
