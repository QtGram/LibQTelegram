#include "messagecopy.h"


MessageCopy::MessageCopy(QObject* parent) : TelegramObject(parent)
{
	this->_orig_message = NULL;
	this->_constructorid = MessageCopy::ctorMsgCopy;
}

void MessageCopy::read(MTProtoStream* mtstream) 
{
	this->_constructorid = mtstream->readTLConstructor();
	
	Q_ASSERT((this->_constructorid == MessageCopy::ctorMsgCopy));
	
	if(this->_constructorid == MessageCopy::ctorMsgCopy)
	{
		RESET_TLTYPE(MTProtoMessage, this->_orig_message);
		this->_orig_message->read(mtstream);
	}
}

void MessageCopy::write(MTProtoStream* mtstream) 
{
	Q_ASSERT((this->_constructorid == MessageCopy::ctorMsgCopy));
	
	mtstream->writeTLConstructor(this->_constructorid);
	
	if(this->_constructorid == MessageCopy::ctorMsgCopy)
	{
		Q_ASSERT(this->_orig_message != NULL);
		this->_orig_message->write(mtstream);
	}
}

MTProtoMessage* MessageCopy::origMessage() const
{
	return this->_orig_message;
}

void MessageCopy::setOrigMessage(MTProtoMessage* orig_message) 
{
	if(this->_orig_message == orig_message)
		return;

	this->_orig_message = orig_message;
    emit origMessageChanged();
}
