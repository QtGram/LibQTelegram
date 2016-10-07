#ifndef ACCOUNTPRIVACYRULES_H
#define ACCOUNTPRIVACYRULES_H

// Generated by APIGenerator 1.0
// DO NOT EDIT!!!

#include "user.h"
#include "../../types/basic.h"
#include "../../types/telegramobject.h"
#include "privacyrule.h"

class AccountPrivacyRules: public TelegramObject
{
	Q_OBJECT

	Q_PROPERTY(TLVector<PrivacyRule*> rules READ rules WRITE setRules NOTIFY rulesChanged)
	Q_PROPERTY(TLVector<User*> users READ users WRITE setUsers NOTIFY usersChanged)

	public:
		enum Constructors {
			ctorAccountPrivacyRules = 0x554abb6f,
		};

	public:
		explicit AccountPrivacyRules(QObject* parent = 0);
		virtual void read(MTProtoStream* mtstream);
		virtual void write(MTProtoStream* mtstream);
	protected:
		virtual void compileFlags();
	public:
		const TLVector<PrivacyRule*>& rules() const;
		void setRules(const TLVector<PrivacyRule*>& rules);
		const TLVector<User*>& users() const;
		void setUsers(const TLVector<User*>& users);

	signals:
		void rulesChanged();
		void usersChanged();

	private:
		TLVector<PrivacyRule*> _rules;
		TLVector<User*> _users;

};

#endif // ACCOUNTPRIVACYRULES_H
