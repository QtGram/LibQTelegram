#include "dialogmodel.h"
#include "../types/telegramhelper.h"

DialogModel::DialogModel(QObject *parent) : TelegramSortFilterProxyModel(parent), _dialog(NULL), _messagesmodel(NULL)
{
    this->_messagesmodel = new MessagesModel(this);
    this->setSourceModel(this->_messagesmodel);
}

Dialog *DialogModel::dialog() const
{
    return this->_dialog;
}

void DialogModel::setDialog(Dialog *dialog)
{
    if(this->_dialog == dialog)
        return;

    this->_dialog = dialog;
    emit dialogChanged();
}

bool DialogModel::filterAcceptsRow(int sourcerow, const QModelIndex&) const
{
    if(!this->_dialog)
        return false;

    Message* message = this->_messagesmodel->data(this->_messagesmodel->index(sourcerow, 0), MessagesModel::ItemRole).value<Message*>();

    if(message)
        return TelegramHelper::identifier(this->_dialog) == TelegramHelper::dialogIdentifier(message);

    return false;
}

void DialogModel::updateTelegram(Telegram *telegram)
{
    this->_messagesmodel->setTelegram(telegram);
}
