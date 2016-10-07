// Generated by APIGenerator 1.0
// DO NOT EDIT!!!

#include "photosize.h"


PhotoSize::PhotoSize(QObject* parent) : TelegramObject(parent)
{
	this->_location = NULL;
	this->_w = 0;
	this->_h = 0;
	this->_size = 0;
}

void PhotoSize::read(MTProtoStream* mtstream) 
{
	this->_constructorid = mtstream->readTLConstructor();
	
	Q_ASSERT((this->_constructorid == PhotoSize::ctorPhotoSizeEmpty) ||
		 (this->_constructorid == PhotoSize::ctorPhotoSize) ||
		 (this->_constructorid == PhotoSize::ctorPhotoCachedSize));
	
	if(this->_constructorid == PhotoSize::ctorPhotoSizeEmpty)
		this->_type = mtstream->readTLString();
	else if(this->_constructorid == PhotoSize::ctorPhotoSize)
	{
		this->_type = mtstream->readTLString();
		TLInt location_ctor = mtstream->peekTLConstructor();
		
		if(location_ctor != TLTypes::Null)
		{
			RESET_TLTYPE(FileLocation, this->_location);
			this->_location->read(mtstream);
		}
		else
		{
			NULL_TLTYPE(this->_location);
			mtstream->readTLConstructor(); // Skip Null
		}
		
		this->_w = mtstream->readTLInt();
		this->_h = mtstream->readTLInt();
		this->_size = mtstream->readTLInt();
	}
	else if(this->_constructorid == PhotoSize::ctorPhotoCachedSize)
	{
		this->_type = mtstream->readTLString();
		TLInt location_ctor = mtstream->peekTLConstructor();
		
		if(location_ctor != TLTypes::Null)
		{
			RESET_TLTYPE(FileLocation, this->_location);
			this->_location->read(mtstream);
		}
		else
		{
			NULL_TLTYPE(this->_location);
			mtstream->readTLConstructor(); // Skip Null
		}
		
		this->_w = mtstream->readTLInt();
		this->_h = mtstream->readTLInt();
		this->_bytes = mtstream->readTLBytes();
	}
}

void PhotoSize::write(MTProtoStream* mtstream) 
{
	Q_ASSERT((this->_constructorid == PhotoSize::ctorPhotoSizeEmpty) ||
		 (this->_constructorid == PhotoSize::ctorPhotoSize) ||
		 (this->_constructorid == PhotoSize::ctorPhotoCachedSize));
	
	this->compileFlags();
	mtstream->writeTLConstructor(this->_constructorid);
	
	if(this->_constructorid == PhotoSize::ctorPhotoSizeEmpty)
		mtstream->writeTLString(this->_type);
	else if(this->_constructorid == PhotoSize::ctorPhotoSize)
	{
		mtstream->writeTLString(this->_type);
		if(this->_location != NULL)
			this->_location->write(mtstream);
		else
			mtstream->writeTLConstructor(TLTypes::Null);
		
		mtstream->writeTLInt(this->_w);
		mtstream->writeTLInt(this->_h);
		mtstream->writeTLInt(this->_size);
	}
	else if(this->_constructorid == PhotoSize::ctorPhotoCachedSize)
	{
		mtstream->writeTLString(this->_type);
		if(this->_location != NULL)
			this->_location->write(mtstream);
		else
			mtstream->writeTLConstructor(TLTypes::Null);
		
		mtstream->writeTLInt(this->_w);
		mtstream->writeTLInt(this->_h);
		mtstream->writeTLBytes(this->_bytes);
	}
}

void PhotoSize::compileFlags() 
{
	
}

TLString PhotoSize::type() const
{
	return this->_type;
}

void PhotoSize::setType(TLString type) 
{
	if(this->_type == type)
		return;

	this->_type = type;
	emit typeChanged();
}

FileLocation* PhotoSize::location() const
{
	return this->_location;
}

void PhotoSize::setLocation(FileLocation* location) 
{
	if(this->_location == location)
		return;

	this->_location = location;
	emit locationChanged();
}

TLInt PhotoSize::w() const
{
	return this->_w;
}

void PhotoSize::setW(TLInt w) 
{
	if(this->_w == w)
		return;

	this->_w = w;
	emit wChanged();
}

TLInt PhotoSize::h() const
{
	return this->_h;
}

void PhotoSize::setH(TLInt h) 
{
	if(this->_h == h)
		return;

	this->_h = h;
	emit hChanged();
}

TLInt PhotoSize::size() const
{
	return this->_size;
}

void PhotoSize::setSize(TLInt size) 
{
	if(this->_size == size)
		return;

	this->_size = size;
	emit sizeChanged();
}

TLBytes PhotoSize::bytes() const
{
	return this->_bytes;
}

void PhotoSize::setBytes(TLBytes bytes) 
{
	if(this->_bytes == bytes)
		return;

	this->_bytes = bytes;
	emit bytesChanged();
}

