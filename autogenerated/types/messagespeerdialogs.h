#ifndef MESSAGESPEERDIALOGS_H
#define MESSAGESPEERDIALOGS_H

// Generated by APIGenerator 1.0
// DO NOT EDIT!!!

#include "message.h"
#include "user.h"
#include "updatesstate.h"
#include "../../types/basic.h"
#include "dialog.h"
#include "../../types/telegramobject.h"
#include "chat.h"

class MessagesPeerDialogs: public TelegramObject
{
	Q_OBJECT

	Q_PROPERTY(TLVector<Dialog*> dialogs READ dialogs WRITE setDialogs NOTIFY dialogsChanged)
	Q_PROPERTY(TLVector<Message*> messages READ messages WRITE setMessages NOTIFY messagesChanged)
	Q_PROPERTY(TLVector<Chat*> chats READ chats WRITE setChats NOTIFY chatsChanged)
	Q_PROPERTY(TLVector<User*> users READ users WRITE setUsers NOTIFY usersChanged)
	Q_PROPERTY(UpdatesState* state READ state WRITE setState NOTIFY stateChanged)

	public:
		enum Constructors {
			ctorMessagesPeerDialogs = 0x3371c354,
		};

	public:
		explicit MessagesPeerDialogs(QObject* parent = 0);
		virtual void read(MTProtoStream* mtstream);
		virtual void write(MTProtoStream* mtstream);
	protected:
		virtual void compileFlags();
	public:
		const TLVector<Dialog*>& dialogs() const;
		void setDialogs(const TLVector<Dialog*>& dialogs);
		const TLVector<Message*>& messages() const;
		void setMessages(const TLVector<Message*>& messages);
		const TLVector<Chat*>& chats() const;
		void setChats(const TLVector<Chat*>& chats);
		const TLVector<User*>& users() const;
		void setUsers(const TLVector<User*>& users);
		UpdatesState* state() const;
		void setState(UpdatesState* state);

	signals:
		void dialogsChanged();
		void messagesChanged();
		void chatsChanged();
		void usersChanged();
		void stateChanged();

	private:
		TLVector<Dialog*> _dialogs;
		TLVector<Message*> _messages;
		TLVector<Chat*> _chats;
		TLVector<User*> _users;
		UpdatesState* _state;

};

#endif // MESSAGESPEERDIALOGS_H
