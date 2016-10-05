#ifndef MESSAGECONTAINER_H
#define MESSAGECONTAINER_H

#include "../../types/basic.h"
#include "../../types/telegramobject.h"
#include "mtprotomessage.h"

class MessageContainer: public TelegramObject
{
	Q_OBJECT

	Q_PROPERTY(TLVector<MTProtoMessage*> messages READ messages WRITE setMessages NOTIFY messagesChanged)

	public:
		enum Constructors {
			ctorMsgContainer = 0x73f1f8dc,
		};

	public:
		explicit MessageContainer(QObject* parent = 0);
		virtual void read(MTProtoStream* mtstream);
		virtual void write(MTProtoStream* mtstream);
		const TLVector<MTProtoMessage*>& messages() const;
		void setMessages(const TLVector<MTProtoMessage*>& messages);

	signals:
		void messagesChanged();

	private:
		TLVector<MTProtoMessage*> _messages;

};

#endif // MESSAGECONTAINER_H
