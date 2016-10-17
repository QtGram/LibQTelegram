#include "qquickmediamessageitem.h"
#include <QDateTime>

QQuickMediaMessageItem::QQuickMediaMessageItem(QQuickItem *parent): QQuickBaseItem(parent), _message(NULL), _size(0)
{
}

Message *QQuickMediaMessageItem::message() const
{
    return this->_message;
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
