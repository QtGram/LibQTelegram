#ifndef NOTIFICATIONOBJECT_H
#define NOTIFICATIONOBJECT_H

#include <QObject>

class NotificationObject : public QObject
{
    Q_OBJECT

    Q_PROPERTY(QString title READ title NOTIFY titleChanged)
    Q_PROPERTY(QString message READ message NOTIFY messageChanged)
    Q_PROPERTY(bool isCurrentDialog READ isCurrentDialog NOTIFY isCurrentDialogChanged)

    public:
        explicit NotificationObject(QObject *parent = 0);
        const QString& title() const;
        const QString& message() const;
        bool isCurrentDialog() const;
        void setTitle(const QString& title);
        void setMessage(const QString& message);
        void setIsCurrentDialog(bool iscurrentdialog);

    signals:
        void titleChanged();
        void messageChanged();
        void isCurrentDialogChanged();

    private:
        QString _title;
        QString _message;
        bool _iscurrentdialog;
};

#endif // NOTIFICATIONOBJECT_H
