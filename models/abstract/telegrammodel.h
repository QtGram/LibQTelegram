#ifndef TELEGRAMMODEL_H
#define TELEGRAMMODEL_H

#include <QAbstractListModel>
#include "../../telegram.h"

class TelegramModel : public QAbstractListModel
{
    Q_OBJECT

    Q_PROPERTY(Telegram* telegram READ telegram WRITE setTelegram NOTIFY telegramChanged)
    Q_PROPERTY(bool loading READ loading NOTIFY loadingChanged)

    public:
        enum Roles { ItemRole = Qt::UserRole };

    public:
        explicit TelegramModel(QObject *parent = 0);
        Telegram* telegram() const;
        void setTelegram(Telegram* telegram);
        bool loading() const;

    protected:
        void setLoading(bool loading);
        virtual QHash<int, QByteArray> initRoles() const;
        virtual void telegramReady() = 0;

    signals:
        void telegramChanged();
        void loadingChanged();

    protected:
        Telegram* _telegram;
        bool _loading;
};

#endif // TELEGRAMMODEL_H
