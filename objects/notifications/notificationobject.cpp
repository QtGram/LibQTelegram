#include "notificationobject.h"

NotificationObject::NotificationObject(QObject *parent) : QObject(parent), _dialog(NULL), _iscurrentdialog(false)
{

}

Dialog *NotificationObject::dialog() const
{
    return this->_dialog;
}

const QString &NotificationObject::title() const
{
    return this->_title;
}

const QString &NotificationObject::message() const
{
    return this->_message;
}

bool NotificationObject::isCurrentDialog() const
{
    return this->_iscurrentdialog;
}

void NotificationObject::setDialog(Dialog *dialog)
{
    if(this->_dialog == dialog)
        return;

    this->_dialog = dialog;
    emit dialogChanged();
}

void NotificationObject::setTitle(const QString &title)
{
    if(this->_title == title)
        return;

    this->_title = title;
    emit titleChanged();
}

void NotificationObject::setMessage(const QString &message)
{
    if(this->_message == message)
        return;

    this->_message = message;
    emit messageChanged();
}

void NotificationObject::setIsCurrentDialog(bool iscurrentdialog)
{
    if(this->_iscurrentdialog == iscurrentdialog)
        return;

    this->_iscurrentdialog = iscurrentdialog;
    emit isCurrentDialogChanged();
}
