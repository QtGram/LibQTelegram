#ifndef QQUICKMEDIAMESSAGEITEM_H
#define QQUICKMEDIAMESSAGEITEM_H

#include "qquickbaseitem.h"

class QQuickMediaMessageItem : public QQuickBaseItem
{
    Q_OBJECT

    Q_PROPERTY(Message* message READ message WRITE setMessage NOTIFY messageChanged)
    Q_PROPERTY(int size READ size WRITE setSize NOTIFY sizeChanged)

    public:
        QQuickMediaMessageItem(QQuickItem *parent = 0);
        Message* message() const;
        int size() const;
        void setMessage(Message* message);
        void setSize(int size);

    private slots:
        void updateMetrics();

    private:
        bool documentIsSticker(Document* document);
        bool documentIsAnimated(Document* document);
        void initialize();

    signals:
        void telegramChanged();
        void messageChanged();
        void sizeChanged();

    private:
        Message* _message;
        int _size;
};

#endif // QQUICKMEDIAMESSAGEITEM_H
