#ifndef FORWARDDIALOGSMODEL_H
#define FORWARDDIALOGSMODEL_H

#include "abstract/telegrammodel.h"

class ForwardDialogsModel : public TelegramModel
{
    Q_OBJECT

    Q_PROPERTY(Dialog* fromDialog READ fromDialog WRITE setFromDialog NOTIFY fromDialogChanged)

    public:
        enum DialogRoles {
            TitleRole = Qt::UserRole + 10,
        };

    public:
        explicit ForwardDialogsModel(QObject *parent = 0);
        Dialog* fromDialog() const;
        void setFromDialog(Dialog* dialog);

    public:
        virtual QVariant data(const QModelIndex &index, int role) const;
        virtual int rowCount(const QModelIndex& = QModelIndex()) const;
        virtual QHash<int, QByteArray> roleNames() const;

    protected:
        virtual void telegramReady();

    signals:
        void fromDialogChanged();

    private:
        QList<Dialog*> _dialogs;
        Dialog* _fromdialog;
};

#endif // FORWARDDIALOGSMODEL_H
