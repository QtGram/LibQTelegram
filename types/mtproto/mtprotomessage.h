#ifndef MTPROTOMESSAGE_H
#define MTPROTOMESSAGE_H

#include "../../types/basic.h"
#include "../../types/telegramobject.h"

class MTProtoMessage: public TelegramObject
{
	Q_OBJECT

	Q_PROPERTY(TLLong msgId READ msgId WRITE setMsgId NOTIFY msgIdChanged)
	Q_PROPERTY(TLInt seqno READ seqno WRITE setSeqno NOTIFY seqnoChanged)
	Q_PROPERTY(TLInt bytes READ bytes WRITE setBytes NOTIFY bytesChanged)
    Q_PROPERTY(TLBytes body READ body WRITE setBody NOTIFY bodyChanged)

	public:
		explicit MTProtoMessage(QObject* parent = 0);
		virtual void read(MTProtoStream* mtstream);
		virtual void write(MTProtoStream* mtstream);
		TLLong msgId() const;
		void setMsgId(TLLong msg_id);
		TLInt seqno() const;
		void setSeqno(TLInt seqno);
		TLInt bytes() const;
		void setBytes(TLInt bytes);
        const TLBytes& body() const;
        void setBody(const TLBytes& body);

	signals:
		void msgIdChanged();
		void seqnoChanged();
		void bytesChanged();
		void bodyChanged();

	private:
		TLLong _msg_id;
		TLInt _seqno;
		TLInt _bytes;
        TLBytes _body;

};

#endif // MTPROTOMESSAGE_H
