#include "notificationobject.h"

NotificationObject::NotificationObject(QObject *parent) : QObject(parent), _dialogid(0), _iscurrentdialog(false)
{

}

TLInt NotificationObject::dialogId() const
{
    return this->_dialogid;
}

TLInt NotificationObject::date() const
{
    return this->_date;
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

void NotificationObject::setDialogId(TLInt dialogid)
{
    if(this->_dialogid == dialogid)
        return;

    this->_dialogid = dialogid;
    emit dialogIdChanged();
}

void NotificationObject::setDate(TLInt date)
{
    if(this->_date == date)
        return;

    this->_date = date;
    emit dateChanged();
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
