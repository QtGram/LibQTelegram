#ifndef DIALOGSMODEL_H
#define DIALOGSMODEL_H

#include "telegrammodel.h"
#include "objects/dialogobject.h"

class DialogsModel : public TelegramModel
{
    Q_OBJECT

    public:
        explicit DialogsModel(QObject *parent = 0);
        virtual QVariant data(const QModelIndex &index, int role) const;
        virtual int rowCount(const QModelIndex &) const;
        virtual QHash<int, QByteArray> roleNames() const;

    private:
        void sortDialogs();

    protected:
        virtual void telegramReady();

    private:
        QList<DialogObject*> _dialogs;
};

#endif // DIALOGSMODEL_H
