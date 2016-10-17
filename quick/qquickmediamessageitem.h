#ifndef QQUICKMEDIAMESSAGEITEM_H
#define QQUICKMEDIAMESSAGEITEM_H

#include "qquickbaseitem.h"

class QQuickMediaMessageItem : public QQuickBaseItem
{
    Q_OBJECT

    Q_PROPERTY(Message* message READ message WRITE setMessage NOTIFY messageChanged)
    Q_PROPERTY(bool isSticker READ isSticker NOTIFY isStickerChanged)
    Q_PROPERTY(bool isAnimated READ isAnimated NOTIFY isAnimatedChanged)
    Q_PROPERTY(int size READ size WRITE setSize NOTIFY sizeChanged)

    public:
        QQuickMediaMessageItem(QQuickItem *parent = 0);
        Message* message() const;
        bool isSticker() const;
        bool isAnimated() const;
        int size() const;
        void setMessage(Message* message);
        void setSize(int size);

    private slots:
        void updateMetrics();

    private:
        bool documentIsSticker(Document* document) const;
        bool documentIsAnimated(Document* document) const;
        void initialize();

    signals:
        void messageChanged();
        void isStickerChanged();
        void isAnimatedChanged();
        void sizeChanged();

    private:
        Message* _message;
        int _size;
};

#endif // QQUICKMEDIAMESSAGEITEM_H
