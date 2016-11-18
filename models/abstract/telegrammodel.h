#ifndef TELEGRAMMODEL_H
#define TELEGRAMMODEL_H

#include <QAbstractListModel>
#include "../../telegram.h"

#define Emit_DataChanged(i) emit dataChanged(this->index(i, 0), this->index(i, 0))
#define Emit_DataChangedRoles(i, roles) emit dataChanged(this->index(i, 0), this->index(i, 0), QVector<int>() << roles)
#define Emit_DataChangedRangeRoles(first, last, roles) emit dataChanged(this->index(first, 0), this->index(last, 0), QVector<int>() << roles)

#define Safe_MoveIdx(srcidx, destidx) (((destidx >= srcidx) && (destidx <= (srcidx + 1))) ? (destidx + 1) : destidx)

class TelegramModel : public QAbstractListModel
{
    Q_OBJECT

    Q_PROPERTY(Telegram* telegram READ telegram WRITE setTelegram NOTIFY telegramChanged)
    Q_PROPERTY(bool initializing READ initializing NOTIFY initializingChanged)
    Q_PROPERTY(bool loading READ loading NOTIFY loadingChanged)

    public:
        enum Roles { ItemRole = Qt::UserRole };

    public:
        explicit TelegramModel(QObject *parent = 0);
        Telegram* telegram() const;
        void setTelegram(Telegram* telegram);
        bool initializing() const;
        bool loading() const;

    protected:
        void setInitializing(bool initializing);
        void setLoading(bool loading);
        virtual QHash<int, QByteArray> initRoles() const;
        virtual void telegramReady() = 0;

    signals:
        void telegramChanged();
        void initializingChanged();
        void loadingChanged();

    protected:
        Telegram* _telegram;
        bool _initializing;
        bool _loading;
};

#endif // TELEGRAMMODEL_H
