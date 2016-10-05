#include "messagecontainer.h"


MessageContainer::MessageContainer(QObject* parent) : TelegramObject(parent)
{
	this->_constructorid = MessageContainer::ctorMsgContainer;
}

void MessageContainer::read(MTProtoStream* mtstream) 
{
	this->_constructorid = mtstream->readTLConstructor();
	
	Q_ASSERT((this->_constructorid == MessageContainer::ctorMsgContainer));
	
	if(this->_constructorid == MessageContainer::ctorMsgContainer)
		mtstream->readTLVector<MTProtoMessage>(this->_messages, true);
}

void MessageContainer::write(MTProtoStream* mtstream) 
{
	Q_ASSERT((this->_constructorid == MessageContainer::ctorMsgContainer));
	
	mtstream->writeTLConstructor(this->_constructorid);
	
	if(this->_constructorid == MessageContainer::ctorMsgContainer)
		mtstream->writeTLVector(this->_messages, true);
}

const TLVector<MTProtoMessage*>& MessageContainer::messages() const
{
	return this->_messages;
}

void MessageContainer::setMessages(const TLVector<MTProtoMessage*>& messages) 
{
	if(this->_messages == messages)
		return;

	this->_messages = messages;
    emit messagesChanged();
}
