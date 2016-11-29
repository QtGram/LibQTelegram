#include "qquickstickerview.h"

QQuickStickerView::QQuickStickerView(QQuickItem *parent) : QQuickBaseItem(parent), _sticker(NULL), _delegate(NULL)
{

}

Document *QQuickStickerView::sticker() const
{
    return this->_sticker;
}

void QQuickStickerView::setSticker(Document *sticker)
{
    if(this->_sticker == sticker)
        return;

    this->_sticker = sticker;
    emit stickerChanged();
}

QQmlComponent *QQuickStickerView::delegate() const
{
    return this->_delegate;
}

void QQuickStickerView::setDelegate(QQmlComponent *delegate)
{
    if(this->_delegate == delegate)
        return;

    this->_delegate = delegate;
    emit delegateChanged();
}

void QQuickStickerView::componentComplete()
{
    QQuickBaseItem::componentComplete();

    if(!this->_sticker || !this->_delegate)
        return;

    this->createFileObject(this->_sticker);
    this->createObject(this->_delegate);
}
