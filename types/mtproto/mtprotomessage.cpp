#include "mtprotomessage.h"


MTProtoMessage::MTProtoMessage(QObject* parent) : TelegramObject(parent)
{
	this->_msg_id = 0;
	this->_seqno = 0;
	this->_bytes = 0;
	this->_body = NULL;
    this->_constructorid = 0;
}

void MTProtoMessage::read(MTProtoStream* mtstream) 
{
	this->_msg_id = mtstream->readTLLong();
	this->_seqno = mtstream->readTLInt();
	this->_bytes = mtstream->readTLInt();

    this->_constructorid = mtstream->peekTLConstructor();
    this->_body = mtstream->readRaw(this->_bytes);
}

void MTProtoMessage::write(MTProtoStream* mtstream) 
{
    /*
    Q_ASSERT((this->_constructorid == MTProtoMessage::ctorMtProtoMessage));
	
	mtstream->writeTLConstructor(this->_constructorid);
	
	if(this->_constructorid == MTProtoMessage::ctorMtProtoMessage)
	{
		mtstream->writeTLLong(this->_msg_id);
		mtstream->writeTLInt(this->_seqno);
		mtstream->writeTLInt(this->_bytes);
		Q_ASSERT(this->_body != NULL);
		this->_body->write(mtstream);
	}
    */
}

TLLong MTProtoMessage::msgId() const
{
	return this->_msg_id;
}

void MTProtoMessage::setMsgId(TLLong msg_id) 
{
	if(this->_msg_id == msg_id)
		return;

	this->_msg_id = msg_id;
	emit msgIdChanged();
}

TLInt MTProtoMessage::seqno() const
{
	return this->_seqno;
}

void MTProtoMessage::setSeqno(TLInt seqno) 
{
	if(this->_seqno == seqno)
		return;

	this->_seqno = seqno;
	emit seqnoChanged();
}

TLInt MTProtoMessage::bytes() const
{
	return this->_bytes;
}

void MTProtoMessage::setBytes(TLInt bytes) 
{
	if(this->_bytes == bytes)
		return;

	this->_bytes = bytes;
	emit bytesChanged();
}

const TLBytes &MTProtoMessage::body() const
{
	return this->_body;
}

void MTProtoMessage::setBody(const TLBytes& body)
{
	if(this->_body == body)
		return;

	this->_body = body;
	emit bodyChanged();
}

