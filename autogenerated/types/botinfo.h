#ifndef BOTINFO_H
#define BOTINFO_H

// Generated by APIGenerator 1.0
// DO NOT EDIT!!!

#include "../../types/telegramobject.h"
#include "../../types/basic.h"
#include "botcommand.h"

class BotInfo: public TelegramObject
{
	Q_OBJECT

	Q_PROPERTY(TLInt userId READ userId WRITE setUserId NOTIFY userIdChanged)
	Q_PROPERTY(TLString description READ description WRITE setDescription NOTIFY descriptionChanged)
	Q_PROPERTY(TLVector<BotCommand*> commands READ commands WRITE setCommands NOTIFY commandsChanged)

	Q_ENUMS(Constructors)

	public:
		enum Constructors {
			ctorBotInfo = 0x98e81d3a,
		};

	public:
		explicit BotInfo(QObject* parent = 0);
		virtual void read(MTProtoStream* mtstream);
		virtual void write(MTProtoStream* mtstream);
	protected:
		virtual void compileFlags();
	public:
		TLInt userId() const;
		void setUserId(TLInt user_id);
		TLString description() const;
		void setDescription(TLString description);
		const TLVector<BotCommand*>& commands() const;
		void setCommands(const TLVector<BotCommand*>& commands);

	signals:
		void userIdChanged();
		void descriptionChanged();
		void commandsChanged();

	private:
		TLInt _user_id;
		TLString _description;
		TLVector<BotCommand*> _commands;

};

#endif // BOTINFO_H
