#ifndef DIALOGMODEL_H
#define DIALOGMODEL_H

#include "abstract/telegramsortfilterproxymodel.h"
#include "messagesmodel.h"

class DialogModel : public TelegramSortFilterProxyModel
{
    Q_OBJECT

    Q_PROPERTY(Dialog* dialog READ dialog WRITE setDialog NOTIFY dialogChanged)

    public:
        explicit DialogModel(QObject *parent = 0);
        Dialog* dialog() const;
        void setDialog(Dialog* dialog);

    protected:
        virtual bool filterAcceptsRow(int source_row, const QModelIndex &) const;
        virtual void updateTelegram(Telegram *telegram);

    signals:
        void dialogChanged();

    private:
        Dialog* _dialog;
        MessagesModel* _messagesmodel;
};

#endif // DIALOGMODEL_H
