#ifndef MSGSSTATEINFO_H
#define MSGSSTATEINFO_H

// Generated by APIGenerator 1.0
// DO NOT EDIT!!!

#include "../../types/telegramobject.h"
#include "../../types/basic.h"

class MsgsStateInfo: public TelegramObject
{
	Q_OBJECT

	Q_PROPERTY(TLLong reqMsgId READ reqMsgId WRITE setReqMsgId NOTIFY reqMsgIdChanged)
	Q_PROPERTY(TLString info READ info WRITE setInfo NOTIFY infoChanged)

	Q_ENUMS(Constructors)

	public:
		enum Constructors {
			ctorMsgsStateInfo = 0x04deb57d,
		};

	public:
		explicit MsgsStateInfo(QObject* parent = 0);
		virtual void read(MTProtoStream* mtstream);
		virtual void write(MTProtoStream* mtstream);
	protected:
		virtual void compileFlags();
	public:
		TLLong reqMsgId() const;
		void setReqMsgId(TLLong req_msg_id);
		TLString info() const;
		void setInfo(TLString info);

	signals:
		void reqMsgIdChanged();
		void infoChanged();

	private:
		TLLong _req_msg_id;
		TLString _info;

};

#endif // MSGSSTATEINFO_H
