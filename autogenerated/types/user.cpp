// Generated by APIGenerator 1.0
// DO NOT EDIT!!!

#include "user.h"


User::User(QObject* parent) : TelegramObject(parent)
{
	this->_id = 0;
	this->_flags = 0;
	this->_is_self = false;
	this->_is_contact = false;
	this->_is_mutual_contact = false;
	this->_is_deleted = false;
	this->_is_bot = false;
	this->_is_bot_chat_history = false;
	this->_is_bot_nochats = false;
	this->_is_verified = false;
	this->_is_restricted = false;
	this->_is_min = false;
	this->_is_bot_inline_geo = false;
	this->_access_hash = 0;
	this->_photo = NULL;
	this->_status = NULL;
	this->_bot_info_version = 0;
}

void User::read(MTProtoStream* mtstream) 
{
	this->_constructorid = mtstream->readTLConstructor();
	
	Q_ASSERT((this->_constructorid == User::CtorUserEmpty) ||
		 (this->_constructorid == User::CtorUser));
	
	if(this->_constructorid == User::CtorUserEmpty)
		this->_id = mtstream->readTLInt();
	else if(this->_constructorid == User::CtorUser)
	{
		this->_flags = mtstream->readTLInt();
		this->_is_self = IS_FLAG_SET(this->_flags, 10);
		this->_is_contact = IS_FLAG_SET(this->_flags, 11);
		this->_is_mutual_contact = IS_FLAG_SET(this->_flags, 12);
		this->_is_deleted = IS_FLAG_SET(this->_flags, 13);
		this->_is_bot = IS_FLAG_SET(this->_flags, 14);
		this->_is_bot_chat_history = IS_FLAG_SET(this->_flags, 15);
		this->_is_bot_nochats = IS_FLAG_SET(this->_flags, 16);
		this->_is_verified = IS_FLAG_SET(this->_flags, 17);
		this->_is_restricted = IS_FLAG_SET(this->_flags, 18);
		this->_is_min = IS_FLAG_SET(this->_flags, 20);
		this->_is_bot_inline_geo = IS_FLAG_SET(this->_flags, 21);
		this->_id = mtstream->readTLInt();
		if(IS_FLAG_SET(this->_flags, 0))
			this->_access_hash = mtstream->readTLLong();
		
		if(IS_FLAG_SET(this->_flags, 1))
			this->_first_name = mtstream->readTLString();
		
		if(IS_FLAG_SET(this->_flags, 2))
			this->_last_name = mtstream->readTLString();
		
		if(IS_FLAG_SET(this->_flags, 3))
			this->_username = mtstream->readTLString();
		
		if(IS_FLAG_SET(this->_flags, 4))
			this->_phone = mtstream->readTLString();
		
		if(IS_FLAG_SET(this->_flags, 5))
		{
			TLInt photo_ctor = mtstream->peekTLConstructor();
			
			if(photo_ctor != TLTypes::Null)
			{
				this->resetTLType<UserProfilePhoto>(&this->_photo);
				this->_photo->read(mtstream);
			}
			else
			{
				this->nullTLType<UserProfilePhoto>(&this->_photo);
				mtstream->readTLConstructor(); // Skip Null
			}
		}
		
		if(IS_FLAG_SET(this->_flags, 6))
		{
			TLInt status_ctor = mtstream->peekTLConstructor();
			
			if(status_ctor != TLTypes::Null)
			{
				this->resetTLType<UserStatus>(&this->_status);
				this->_status->read(mtstream);
			}
			else
			{
				this->nullTLType<UserStatus>(&this->_status);
				mtstream->readTLConstructor(); // Skip Null
			}
		}
		
		if(IS_FLAG_SET(this->_flags, 14))
			this->_bot_info_version = mtstream->readTLInt();
		
		if(IS_FLAG_SET(this->_flags, 18))
			this->_restriction_reason = mtstream->readTLString();
		
		if(IS_FLAG_SET(this->_flags, 19))
			this->_bot_inline_placeholder = mtstream->readTLString();
	}
}

void User::write(MTProtoStream* mtstream) 
{
	Q_ASSERT((this->_constructorid == User::CtorUserEmpty) ||
		 (this->_constructorid == User::CtorUser));
	
	this->compileFlags();
	mtstream->writeTLConstructor(this->_constructorid);
	
	if(this->_constructorid == User::CtorUserEmpty)
		mtstream->writeTLInt(this->_id);
	else if(this->_constructorid == User::CtorUser)
	{
		mtstream->writeTLInt(this->_flags);
		mtstream->writeTLInt(this->_id);
		if(IS_FLAG_SET(this->_flags, 0))
			mtstream->writeTLLong(this->_access_hash);
		
		if(IS_FLAG_SET(this->_flags, 1))
			mtstream->writeTLString(this->_first_name);
		
		if(IS_FLAG_SET(this->_flags, 2))
			mtstream->writeTLString(this->_last_name);
		
		if(IS_FLAG_SET(this->_flags, 3))
			mtstream->writeTLString(this->_username);
		
		if(IS_FLAG_SET(this->_flags, 4))
			mtstream->writeTLString(this->_phone);
		
		if(IS_FLAG_SET(this->_flags, 5))
		{
			if(this->_photo != NULL)
				this->_photo->write(mtstream);
			else
				mtstream->writeTLConstructor(TLTypes::Null);
		}
		
		if(IS_FLAG_SET(this->_flags, 6))
		{
			if(this->_status != NULL)
				this->_status->write(mtstream);
			else
				mtstream->writeTLConstructor(TLTypes::Null);
		}
		
		if(IS_FLAG_SET(this->_flags, 14))
			mtstream->writeTLInt(this->_bot_info_version);
		
		if(IS_FLAG_SET(this->_flags, 18))
			mtstream->writeTLString(this->_restriction_reason);
		
		if(IS_FLAG_SET(this->_flags, 19))
			mtstream->writeTLString(this->_bot_inline_placeholder);
	}
}

void User::compileFlags() 
{
	this->_flags = 0;
	
	if(this->_constructorid == User::CtorUser)
	{
		if(this->_is_self)
			SET_FLAG_BIT(this->_flags, 10);
		if(this->_is_contact)
			SET_FLAG_BIT(this->_flags, 11);
		if(this->_is_mutual_contact)
			SET_FLAG_BIT(this->_flags, 12);
		if(this->_is_deleted)
			SET_FLAG_BIT(this->_flags, 13);
		if(this->_is_bot)
			SET_FLAG_BIT(this->_flags, 14);
		if(this->_is_bot_chat_history)
			SET_FLAG_BIT(this->_flags, 15);
		if(this->_is_bot_nochats)
			SET_FLAG_BIT(this->_flags, 16);
		if(this->_is_verified)
			SET_FLAG_BIT(this->_flags, 17);
		if(this->_is_restricted)
			SET_FLAG_BIT(this->_flags, 18);
		if(this->_is_min)
			SET_FLAG_BIT(this->_flags, 20);
		if(this->_is_bot_inline_geo)
			SET_FLAG_BIT(this->_flags, 21);
		if(this->_access_hash)
			SET_FLAG_BIT(this->_flags, 0);
		if(!this->_first_name.isEmpty())
			SET_FLAG_BIT(this->_flags, 1);
		if(!this->_last_name.isEmpty())
			SET_FLAG_BIT(this->_flags, 2);
		if(!this->_username.isEmpty())
			SET_FLAG_BIT(this->_flags, 3);
		if(!this->_phone.isEmpty())
			SET_FLAG_BIT(this->_flags, 4);
		if(this->_photo)
			SET_FLAG_BIT(this->_flags, 5);
		if(this->_status)
			SET_FLAG_BIT(this->_flags, 6);
		if(this->_bot_info_version)
			SET_FLAG_BIT(this->_flags, 14);
		if(!this->_restriction_reason.isEmpty())
			SET_FLAG_BIT(this->_flags, 18);
		if(!this->_bot_inline_placeholder.isEmpty())
			SET_FLAG_BIT(this->_flags, 19);
	}
}

TLInt User::id() const
{
	return this->_id;
}

void User::setId(TLInt id) 
{
	if(this->_id == id)
		return;

	this->_id = id;
	emit idChanged();
}

TLInt User::flags() const
{
	return this->_flags;
}

void User::setFlags(TLInt flags) 
{
	if(this->_flags == flags)
		return;

	this->_flags = flags;
	emit flagsChanged();
}

TLTrue User::isSelf() const
{
	return this->_is_self;
}

void User::setIsSelf(TLTrue is_self) 
{
	if(this->_is_self == is_self)
		return;

	this->_is_self = is_self;
	emit isSelfChanged();
}

TLTrue User::isContact() const
{
	return this->_is_contact;
}

void User::setIsContact(TLTrue is_contact) 
{
	if(this->_is_contact == is_contact)
		return;

	this->_is_contact = is_contact;
	emit isContactChanged();
}

TLTrue User::isMutualContact() const
{
	return this->_is_mutual_contact;
}

void User::setIsMutualContact(TLTrue is_mutual_contact) 
{
	if(this->_is_mutual_contact == is_mutual_contact)
		return;

	this->_is_mutual_contact = is_mutual_contact;
	emit isMutualContactChanged();
}

TLTrue User::isDeleted() const
{
	return this->_is_deleted;
}

void User::setIsDeleted(TLTrue is_deleted) 
{
	if(this->_is_deleted == is_deleted)
		return;

	this->_is_deleted = is_deleted;
	emit isDeletedChanged();
}

TLTrue User::isBot() const
{
	return this->_is_bot;
}

void User::setIsBot(TLTrue is_bot) 
{
	if(this->_is_bot == is_bot)
		return;

	this->_is_bot = is_bot;
	emit isBotChanged();
}

TLTrue User::isBotChatHistory() const
{
	return this->_is_bot_chat_history;
}

void User::setIsBotChatHistory(TLTrue is_bot_chat_history) 
{
	if(this->_is_bot_chat_history == is_bot_chat_history)
		return;

	this->_is_bot_chat_history = is_bot_chat_history;
	emit isBotChatHistoryChanged();
}

TLTrue User::isBotNochats() const
{
	return this->_is_bot_nochats;
}

void User::setIsBotNochats(TLTrue is_bot_nochats) 
{
	if(this->_is_bot_nochats == is_bot_nochats)
		return;

	this->_is_bot_nochats = is_bot_nochats;
	emit isBotNochatsChanged();
}

TLTrue User::isVerified() const
{
	return this->_is_verified;
}

void User::setIsVerified(TLTrue is_verified) 
{
	if(this->_is_verified == is_verified)
		return;

	this->_is_verified = is_verified;
	emit isVerifiedChanged();
}

TLTrue User::isRestricted() const
{
	return this->_is_restricted;
}

void User::setIsRestricted(TLTrue is_restricted) 
{
	if(this->_is_restricted == is_restricted)
		return;

	this->_is_restricted = is_restricted;
	emit isRestrictedChanged();
}

TLTrue User::isMin() const
{
	return this->_is_min;
}

void User::setIsMin(TLTrue is_min) 
{
	if(this->_is_min == is_min)
		return;

	this->_is_min = is_min;
	emit isMinChanged();
}

TLTrue User::isBotInlineGeo() const
{
	return this->_is_bot_inline_geo;
}

void User::setIsBotInlineGeo(TLTrue is_bot_inline_geo) 
{
	if(this->_is_bot_inline_geo == is_bot_inline_geo)
		return;

	this->_is_bot_inline_geo = is_bot_inline_geo;
	emit isBotInlineGeoChanged();
}

TLLong User::accessHash() const
{
	return this->_access_hash;
}

void User::setAccessHash(TLLong access_hash) 
{
	if(this->_access_hash == access_hash)
		return;

	this->_access_hash = access_hash;
	emit accessHashChanged();
}

TLString User::firstName() const
{
	return this->_first_name;
}

void User::setFirstName(TLString first_name) 
{
	if(this->_first_name == first_name)
		return;

	this->_first_name = first_name;
	emit firstNameChanged();
}

TLString User::lastName() const
{
	return this->_last_name;
}

void User::setLastName(TLString last_name) 
{
	if(this->_last_name == last_name)
		return;

	this->_last_name = last_name;
	emit lastNameChanged();
}

TLString User::username() const
{
	return this->_username;
}

void User::setUsername(TLString username) 
{
	if(this->_username == username)
		return;

	this->_username = username;
	emit usernameChanged();
}

TLString User::phone() const
{
	return this->_phone;
}

void User::setPhone(TLString phone) 
{
	if(this->_phone == phone)
		return;

	this->_phone = phone;
	emit phoneChanged();
}

UserProfilePhoto* User::photo() const
{
	return this->_photo;
}

void User::setPhoto(UserProfilePhoto* photo) 
{
	if(this->_photo == photo)
		return;

	this->deleteChild(this->_photo);
	this->_photo = photo;

	if(this->_photo)
		this->_photo->setParent(this);

	emit photoChanged();
}

UserStatus* User::status() const
{
	return this->_status;
}

void User::setStatus(UserStatus* status) 
{
	if(this->_status == status)
		return;

	this->deleteChild(this->_status);
	this->_status = status;

	if(this->_status)
		this->_status->setParent(this);

	emit statusChanged();
}

TLInt User::botInfoVersion() const
{
	return this->_bot_info_version;
}

void User::setBotInfoVersion(TLInt bot_info_version) 
{
	if(this->_bot_info_version == bot_info_version)
		return;

	this->_bot_info_version = bot_info_version;
	emit botInfoVersionChanged();
}

TLString User::restrictionReason() const
{
	return this->_restriction_reason;
}

void User::setRestrictionReason(TLString restriction_reason) 
{
	if(this->_restriction_reason == restriction_reason)
		return;

	this->_restriction_reason = restriction_reason;
	emit restrictionReasonChanged();
}

TLString User::botInlinePlaceholder() const
{
	return this->_bot_inline_placeholder;
}

void User::setBotInlinePlaceholder(TLString bot_inline_placeholder) 
{
	if(this->_bot_inline_placeholder == bot_inline_placeholder)
		return;

	this->_bot_inline_placeholder = bot_inline_placeholder;
	emit botInlinePlaceholderChanged();
}

