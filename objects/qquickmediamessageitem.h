#ifndef QQUICKMEDIAMESSAGEITEM_H
#define QQUICKMEDIAMESSAGEITEM_H

#include <QQuickItem>
#include "../telegram.h"

class QQuickMediaMessageItem : public QQuickItem
{
    Q_OBJECT

    Q_PROPERTY(QString version READ version WRITE setVersion NOTIFY versionChanged)
    Q_PROPERTY(Telegram* telegram READ telegram WRITE setTelegram NOTIFY telegramChanged)
    Q_PROPERTY(Message* message READ message WRITE setMessage NOTIFY messageChanged)
    Q_PROPERTY(int size READ size WRITE setSize NOTIFY sizeChanged)
    Q_PROPERTY(QSize imageSize READ imageSize NOTIFY imageSizeChanged)

    public:
        QQuickMediaMessageItem(QQuickItem *parent = 0);
        QString version() const;
        Telegram* telegram() const;
        Message* message() const;
        int size() const;
        QSize imageSize() const;
        void setVersion(const QString& version);
        void setTelegram(Telegram* telegram);
        void setSize(int size);
        void setMessage(Message* message);

    public slots:
        void download();

    private slots:
        void updateMetrics();

    private:
        QQuickItem *createComponent(const QString& componentcode);
        bool documentIsSticker(Document* document);
        bool documentIsAnimated(Document* document);
        void initialize();
        void createImageElement();
        void createAnimatedElement();
        void applySource();

    signals:
        void versionChanged();
        void telegramChanged();
        void messageChanged();
        void sizeChanged();
        void imageSizeChanged();

    private:
        QQuickItem* _mediaelement;
        FileObject* _fileobject;
        Telegram* _telegram;
        Message* _message;
        QString _version;
        int _size;
};

#endif // QQUICKMEDIAMESSAGEITEM_H
