#include "rpcresult.h"


RpcResult::RpcResult(QObject* parent) : TelegramObject(parent)
{
	this->_req_msg_id = 0;
	this->_constructorid = RpcResult::ctorRpcResult;
}

void RpcResult::read(MTProtoStream* mtstream) 
{
	this->_constructorid = mtstream->readTLConstructor();
	
	Q_ASSERT((this->_constructorid == RpcResult::ctorRpcResult));
	
	if(this->_constructorid == RpcResult::ctorRpcResult)
	{
		this->_req_msg_id = mtstream->readTLLong();
        this->_result = mtstream->readAll();
	}
}

void RpcResult::write(MTProtoStream* mtstream) 
{
    /*
	Q_ASSERT((this->_constructorid == RpcResult::ctorRpcResult));
	
	mtstream->writeTLConstructor(this->_constructorid);
	
	if(this->_constructorid == RpcResult::ctorRpcResult)
	{
		mtstream->writeTLLong(this->_req_msg_id);
		Q_ASSERT(this->_result != NULL);
		this->_result->write(mtstream);
	}
    */
}

TLLong RpcResult::reqMsgId() const
{
	return this->_req_msg_id;
}

void RpcResult::setReqMsgId(TLLong req_msg_id) 
{
	if(this->_req_msg_id == req_msg_id)
		return;

	this->_req_msg_id = req_msg_id;
	emit reqMsgIdChanged();
}

TLBytes RpcResult::result() const
{
	return this->_result;
}

void RpcResult::setResult(TLBytes result) 
{
	if(this->_result == result)
		return;

	this->_result = result;
	emit resultChanged();
}

