#include "qquickwaveform.h"
#include "../types/telegramhelper.h"
#include <QSGSimpleRectNode>
#include <QSGClipNode>
#include <QSGNode>

#define WaveformSkip  1  // px
#define WaveformWidth 2  // px

QQuickWaveform::QQuickWaveform(QQuickItem *parent): QQuickItem(parent), _wavemax(0), _barwidth(WaveformWidth), _barcolor("black"), _message(NULL)
{
    this->setFlag(QQuickItem::ItemHasContents);

    connect(this, &QQuickWaveform::barWidthChanged, this, &QQuickWaveform::contentWidth);
}

qreal QQuickWaveform::contentWidth() const
{
    return (this->_barwidth + WaveformSkip) * this->_waveform.length();
}

int QQuickWaveform::barWidth() const
{
    return this->_barwidth;
}

void QQuickWaveform::setBarWidth(int barwidth)
{
    if(this->_barwidth == barwidth)
        return;

    this->_barwidth = barwidth;
    emit barWidthChanged();
}

QColor QQuickWaveform::barColor() const
{
    return this->_barcolor;
}

void QQuickWaveform::setBarColor(const QColor &barcolor)
{
    if(this->_barcolor == barcolor)
        return;

    this->_barcolor = barcolor;
    emit barColorChanged();
}

Message *QQuickWaveform::message() const
{
    return this->_message;
}

void QQuickWaveform::setMessage(Message *message)
{
    if(this->_message == message)
        return;

    this->_message = message;
    this->findWaveform(message);
    emit messageChanged();
}

void QQuickWaveform::waveRect(int i, QRectF &rect) const
{
    qreal x = i * (this->_barwidth + WaveformSkip), h = this->height();
    qreal barheight = h * (this->_waveform[i] / static_cast<qreal>(this->_wavemax));

    rect.setLeft(x);
    rect.setRight(x + this->_barwidth);
    rect.setTop(h - barheight);
    rect.setBottom(h);
}

void QQuickWaveform::decodeWaveform(const QByteArray &encoded5bit)
{
    if(!this->_waveform.isEmpty())
    {
        this->_wavemax = 0;
        this->_waveform.clear();
    }

    this->_waveform.fill(0x00, (encoded5bit.size() * 8) / 5);

    for(qint32 i = 0, l = this->_waveform.size(); i < l; i++)
    {
        qint32 byte = (i * 5) / 8, shift = (i * 5) % 8;
        uchar wave = ((*(reinterpret_cast<const quint16*>(encoded5bit.constData() + byte)) >> shift) & 0x1F);

        if(wave > this->_wavemax)
            this->_wavemax = wave;

        this->_waveform[i] = wave;
    }

    this->setImplicitWidth(this->contentWidth());
}

void QQuickWaveform::findWaveform(TelegramObject *telegramobject)
{
    if(!telegramobject)
        return;

    if(telegramobject->constructorId() == TLTypes::Document)
    {
        DocumentAttribute* attribute = NULL;
        Document* document = qobject_cast<Document*>(telegramobject);

        if((attribute = TelegramHelper::documentHas(document, TLTypes::DocumentAttributeAudio)))
            this->decodeWaveform(attribute->waveform());
    }
    else if(telegramobject->constructorId() == TLTypes::MessageMediaDocument)
        this->findWaveform(qobject_cast<MessageMedia*>(telegramobject)->document());
    else if(telegramobject->constructorId() == TLTypes::Message)
        this->findWaveform(qobject_cast<Message*>(telegramobject)->media());
}

QSGNode *QQuickWaveform::updatePaintNode(QSGNode* oldnode, QQuickItem::UpdatePaintNodeData*)
{
    QSGClipNode* rootnode = dynamic_cast<QSGClipNode*>(oldnode);

    if(!this->_wavemax)
        return rootnode;

    QRectF cliprect(0, 0, this->width(), this->height());

    if(!rootnode)
    {
        rootnode = new QSGClipNode();
        rootnode->setIsRectangular(true);
        rootnode->setClipRect(cliprect);

        for(int i = 0; i < this->_waveform.length(); i++)
        {
            QRectF rect;
            this->waveRect(i, rect);

            QSGSimpleRectNode* wavenode = new QSGSimpleRectNode();
            wavenode->setColor(this->_barcolor);
            wavenode->setRect(rect);

            rootnode->appendChildNode(wavenode);
        }
    }
    else
    {
        rootnode->setClipRect(cliprect);
        QSGNode* node = rootnode->firstChild();

        for(int i = 0; i < this->_waveform.length(); i++)
        {
            QRectF rect;
            this->waveRect(i, rect);

            dynamic_cast<QSGSimpleRectNode*>(node)->setColor(this->_barcolor);
            dynamic_cast<QSGSimpleRectNode*>(node)->setRect(rect);
            node = node->nextSibling();
        }
    }

    return rootnode;
}
