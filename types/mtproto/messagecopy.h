#ifndef MESSAGECOPY_H
#define MESSAGECOPY_H

#include "../../types/telegramobject.h"
#include "../../types/basic.h"
#include "mtprotomessage.h"

class MessageCopy: public TelegramObject
{
	Q_OBJECT

	Q_PROPERTY(MTProtoMessage* origMessage READ origMessage WRITE setOrigMessage NOTIFY origMessageChanged)

	public:
		enum Constructors {
			ctorMsgCopy = 0xe06046b2,
		};

	public:
		explicit MessageCopy(QObject* parent = 0);
		virtual void read(MTProtoStream* mtstream);
		virtual void write(MTProtoStream* mtstream);
		MTProtoMessage* origMessage() const;
		void setOrigMessage(MTProtoMessage* orig_message);

	signals:
		void origMessageChanged();

	private:
		MTProtoMessage* _orig_message;

};

#endif // MESSAGECOPY_H
