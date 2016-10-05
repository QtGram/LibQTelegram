#ifndef RPCRESULT_H
#define RPCRESULT_H

#include "../../types/basic.h"
#include "../../types/telegramobject.h"

class RpcResult: public TelegramObject
{
	Q_OBJECT

	Q_PROPERTY(TLLong reqMsgId READ reqMsgId WRITE setReqMsgId NOTIFY reqMsgIdChanged)
    Q_PROPERTY(TLBytes result READ result WRITE setResult NOTIFY resultChanged)

	public:
		enum Constructors {
			ctorRpcResult = 0xf35c6d01,
		};

	public:
		explicit RpcResult(QObject* parent = 0);
		virtual void read(MTProtoStream* mtstream);
		virtual void write(MTProtoStream* mtstream);
		TLLong reqMsgId() const;
		void setReqMsgId(TLLong req_msg_id);
        TLBytes result() const;
        void setResult(TLBytes result);

	signals:
		void reqMsgIdChanged();
		void resultChanged();

	private:
		TLLong _req_msg_id;
        TLBytes _result;

};

#endif // RPCRESULT_H
