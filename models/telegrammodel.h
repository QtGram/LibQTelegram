#ifndef TELEGRAMMODEL_H
#define TELEGRAMMODEL_H

#include <QAbstractListModel>
#include "../telegram.h"

#define DELETE_DATA(data) \
    qDeleteAll(data); \
    data.clear();

class TelegramModel : public QAbstractListModel
{
    Q_OBJECT

    Q_PROPERTY(Telegram* telegram READ telegram WRITE setTelegram NOTIFY telegramChanged)

    public:
        enum Roles { ItemRole = Qt::UserRole };

    public:
        explicit TelegramModel(QObject *parent = 0);
        Telegram* telegram() const;
        void setTelegram(Telegram* telegram);

    protected:
        QHash<int, QByteArray> initRoles() const;
        virtual void telegramReady() = 0;

    signals:
        void telegramChanged();

    protected:
        Telegram* _telegram;
};

#endif // TELEGRAMMODEL_H
