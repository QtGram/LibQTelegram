#ifndef TELEGRAMSORTFILTERPROXYMODEL_H
#define TELEGRAMSORTFILTERPROXYMODEL_H

#include <QSortFilterProxyModel>
#include "../../telegram.h"

class TelegramSortFilterProxyModel : public QSortFilterProxyModel
{
    Q_OBJECT

    Q_PROPERTY(Telegram* telegram READ telegram WRITE setTelegram NOTIFY telegramChanged)

    public:
        explicit TelegramSortFilterProxyModel(QObject *parent = 0);
        Telegram* telegram() const;
        void setTelegram(Telegram* telegram);

    protected:
        virtual void updateTelegram(Telegram* telegram) = 0;

    signals:
        void telegramChanged();

    protected:
        Telegram* _telegram;
};

#endif // TELEGRAMSORTFILTERPROXYMODEL_H
