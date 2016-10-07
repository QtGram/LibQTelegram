#ifndef AUTHSENTCODE_H
#define AUTHSENTCODE_H

// Generated by APIGenerator 1.0
// DO NOT EDIT!!!

#include "authcodetype.h"
#include "../../types/basic.h"
#include "authsentcodetype.h"
#include "../../types/telegramobject.h"

class AuthSentCode: public TelegramObject
{
	Q_OBJECT

	Q_PROPERTY(TLInt flags READ flags WRITE setFlags NOTIFY flagsChanged)
	Q_PROPERTY(TLTrue isPhoneRegistered READ isPhoneRegistered WRITE setIsPhoneRegistered NOTIFY isPhoneRegisteredChanged)
	Q_PROPERTY(AuthSentCodeType* type READ type WRITE setType NOTIFY typeChanged)
	Q_PROPERTY(TLString phoneCodeHash READ phoneCodeHash WRITE setPhoneCodeHash NOTIFY phoneCodeHashChanged)
	Q_PROPERTY(AuthCodeType* nextType READ nextType WRITE setNextType NOTIFY nextTypeChanged)
	Q_PROPERTY(TLInt timeout READ timeout WRITE setTimeout NOTIFY timeoutChanged)

	public:
		enum Constructors {
			ctorAuthSentCode = 0x5e002502,
		};

	public:
		explicit AuthSentCode(QObject* parent = 0);
		virtual void read(MTProtoStream* mtstream);
		virtual void write(MTProtoStream* mtstream);
	protected:
		virtual void compileFlags();
	public:
		TLInt flags() const;
		void setFlags(TLInt flags);
		TLTrue isPhoneRegistered() const;
		void setIsPhoneRegistered(TLTrue is_phone_registered);
		AuthSentCodeType* type() const;
		void setType(AuthSentCodeType* type);
		TLString phoneCodeHash() const;
		void setPhoneCodeHash(TLString phone_code_hash);
		AuthCodeType* nextType() const;
		void setNextType(AuthCodeType* next_type);
		TLInt timeout() const;
		void setTimeout(TLInt timeout);

	signals:
		void flagsChanged();
		void isPhoneRegisteredChanged();
		void typeChanged();
		void phoneCodeHashChanged();
		void nextTypeChanged();
		void timeoutChanged();

	private:
		TLInt _flags;
		TLTrue _is_phone_registered;
		AuthSentCodeType* _type;
		TLString _phone_code_hash;
		AuthCodeType* _next_type;
		TLInt _timeout;

};

#endif // AUTHSENTCODE_H
