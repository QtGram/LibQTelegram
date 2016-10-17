#include "qquickmediamessageitem.h"
#include <QDateTime>

QQuickMediaMessageItem::QQuickMediaMessageItem(QQuickItem *parent): QQuickBaseItem(parent), _message(NULL), _size(0)
{
    connect(this, &QQuickMediaMessageItem::messageChanged, this, &QQuickMediaMessageItem::isStickerChanged);
    connect(this, &QQuickMediaMessageItem::messageChanged, this, &QQuickMediaMessageItem::isAnimatedChanged);
}

Message *QQuickMediaMessageItem::message() const
{
    return this->_message;
}

bool QQuickMediaMessageItem::isSticker() const
{
    if(!this->_message || !this->_message->media())
        return false;

    MessageMedia* messagemedia = this->_message->media();

    if(messagemedia->constructorId() != TLTypes::MessageMediaDocument)
        return false;

    return this->documentIsSticker(messagemedia->document());
}

bool QQuickMediaMessageItem::isAnimated() const
{
    if(!this->_message || !this->_message->media())
        return false;

    MessageMedia* messagemedia = this->_message->media();

    if(messagemedia->constructorId() != TLTypes::MessageMediaDocument)
        return false;

    return this->documentIsAnimated(messagemedia->document());
}

int QQuickMediaMessageItem::size() const
{
    return this->_size;
}

void QQuickMediaMessageItem::setMessage(Message *message)
{
    if(this->_message == message)
        return;

    this->_message = message;
    this->initialize();
    emit messageChanged();
}

void QQuickMediaMessageItem::setSize(int size)
{
    if(this->_size == size)
        return;

    this->_size = size;
    this->updateMetrics();
    emit sizeChanged();
}

bool QQuickMediaMessageItem::documentIsSticker(Document *document) const
{
    foreach(DocumentAttribute* attribute, document->attributes())
    {
        if(attribute->constructorId() == TLTypes::DocumentAttributeSticker)
            return true;
    }

    return false;
}

bool QQuickMediaMessageItem::documentIsAnimated(Document *document) const
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
    if(!this->_message || this->_mediaelement)
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
    else
        return;

    FileObject* fileobject = this->createFileObject(this->_message);
    connect(fileobject, &FileObject::imageSizeChanged, this, &QQuickMediaMessageItem::updateMetrics);

    this->updateMetrics();
    this->bindToElement();
}

void QQuickMediaMessageItem::updateMetrics()
{
    if(!this->_size)
        return;

    QSize imagesize = this->imageSize();

    if(!imagesize.isValid())
        return;

    qreal aspectratio = imagesize.width() / static_cast<qreal>(imagesize.height());

    this->setWidth(this->_size);
    this->setHeight(aspectratio ? (this->_size / aspectratio) : 0);
}
