#ifndef CLIENTDHINNERDATA_H
#define CLIENTDHINNERDATA_H

// Generated by APIGenerator 1.0
// DO NOT EDIT!!!

#include "../../types/telegramobject.h"
#include "../../types/basic.h"

class ClientDHInnerData: public TelegramObject
{
	Q_OBJECT

	Q_PROPERTY(TLInt128 nonce READ nonce WRITE setNonce NOTIFY nonceChanged)
	Q_PROPERTY(TLInt128 serverNonce READ serverNonce WRITE setServerNonce NOTIFY serverNonceChanged)
	Q_PROPERTY(TLLong retryId READ retryId WRITE setRetryId NOTIFY retryIdChanged)
	Q_PROPERTY(TLString gB READ gB WRITE setGB NOTIFY gBChanged)

	Q_ENUMS(Constructors)

	public:
		enum Constructors {
			ctorClientDHInnerData = 0x6643b654,
		};

	public:
		explicit ClientDHInnerData(QObject* parent = 0);
		virtual void read(MTProtoStream* mtstream);
		virtual void write(MTProtoStream* mtstream);
	protected:
		virtual void compileFlags();
	public:
		TLInt128 nonce() const;
		void setNonce(TLInt128 nonce);
		TLInt128 serverNonce() const;
		void setServerNonce(TLInt128 server_nonce);
		TLLong retryId() const;
		void setRetryId(TLLong retry_id);
		TLString gB() const;
		void setGB(TLString g_b);

	signals:
		void nonceChanged();
		void serverNonceChanged();
		void retryIdChanged();
		void gBChanged();

	private:
		TLInt128 _nonce;
		TLInt128 _server_nonce;
		TLLong _retry_id;
		TLString _g_b;

};

#endif // CLIENTDHINNERDATA_H
