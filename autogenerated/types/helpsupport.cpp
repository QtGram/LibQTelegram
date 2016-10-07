// Generated by APIGenerator 1.0
// DO NOT EDIT!!!

#include "helpsupport.h"


HelpSupport::HelpSupport(QObject* parent) : TelegramObject(parent)
{
	this->_user = NULL;
	this->_constructorid = HelpSupport::ctorHelpSupport;
}

void HelpSupport::read(MTProtoStream* mtstream) 
{
	this->_constructorid = mtstream->readTLConstructor();
	
	Q_ASSERT((this->_constructorid == HelpSupport::ctorHelpSupport));
	
	if(this->_constructorid == HelpSupport::ctorHelpSupport)
	{
		this->_phone_number = mtstream->readTLString();
		TLInt user_ctor = mtstream->peekTLConstructor();
		
		if(user_ctor != TLTypes::Null)
		{
			RESET_TLTYPE(User, this->_user);
			this->_user->read(mtstream);
		}
		else
		{
			NULL_TLTYPE(this->_user);
			mtstream->readTLConstructor(); // Skip Null
		}
	}
}

void HelpSupport::write(MTProtoStream* mtstream) 
{
	Q_ASSERT((this->_constructorid == HelpSupport::ctorHelpSupport));
	
	this->compileFlags();
	mtstream->writeTLConstructor(this->_constructorid);
	
	if(this->_constructorid == HelpSupport::ctorHelpSupport)
	{
		mtstream->writeTLString(this->_phone_number);
		if(this->_user != NULL)
			this->_user->write(mtstream);
		else
			mtstream->writeTLConstructor(TLTypes::Null);
	}
}

void HelpSupport::compileFlags() 
{
	
}

TLString HelpSupport::phoneNumber() const
{
	return this->_phone_number;
}

void HelpSupport::setPhoneNumber(TLString phone_number) 
{
	if(this->_phone_number == phone_number)
		return;

	this->_phone_number = phone_number;
	emit phoneNumberChanged();
}

User* HelpSupport::user() const
{
	return this->_user;
}

void HelpSupport::setUser(User* user) 
{
	if(this->_user == user)
		return;

	this->_user = user;
	emit userChanged();
}

