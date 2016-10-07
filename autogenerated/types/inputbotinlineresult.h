#ifndef INPUTBOTINLINERESULT_H
#define INPUTBOTINLINERESULT_H

// Generated by APIGenerator 1.0
// DO NOT EDIT!!!

#include "inputdocument.h"
#include "inputphoto.h"
#include "../../types/basic.h"
#include "inputbotinlinemessage.h"
#include "../../types/telegramobject.h"

class InputBotInlineResult: public TelegramObject
{
	Q_OBJECT

	Q_PROPERTY(TLInt flags READ flags WRITE setFlags NOTIFY flagsChanged)
	Q_PROPERTY(TLString id READ id WRITE setId NOTIFY idChanged)
	Q_PROPERTY(TLString type READ type WRITE setType NOTIFY typeChanged)
	Q_PROPERTY(TLString title READ title WRITE setTitle NOTIFY titleChanged)
	Q_PROPERTY(TLString description READ description WRITE setDescription NOTIFY descriptionChanged)
	Q_PROPERTY(TLString url READ url WRITE setUrl NOTIFY urlChanged)
	Q_PROPERTY(TLString thumbUrl READ thumbUrl WRITE setThumbUrl NOTIFY thumbUrlChanged)
	Q_PROPERTY(TLString contentUrl READ contentUrl WRITE setContentUrl NOTIFY contentUrlChanged)
	Q_PROPERTY(TLString contentType READ contentType WRITE setContentType NOTIFY contentTypeChanged)
	Q_PROPERTY(TLInt w READ w WRITE setW NOTIFY wChanged)
	Q_PROPERTY(TLInt h READ h WRITE setH NOTIFY hChanged)
	Q_PROPERTY(TLInt duration READ duration WRITE setDuration NOTIFY durationChanged)
	Q_PROPERTY(InputBotInlineMessage* sendMessage READ sendMessage WRITE setSendMessage NOTIFY sendMessageChanged)
	Q_PROPERTY(InputPhoto* photo READ photo WRITE setPhoto NOTIFY photoChanged)
	Q_PROPERTY(InputDocument* document READ document WRITE setDocument NOTIFY documentChanged)
	Q_PROPERTY(TLString shortName READ shortName WRITE setShortName NOTIFY shortNameChanged)

	public:
		enum Constructors {
			ctorInputBotInlineResult = 0x2cbbe15a,
			ctorInputBotInlineResultPhoto = 0xa8d864a7,
			ctorInputBotInlineResultDocument = 0xfff8fdc4,
			ctorInputBotInlineResultGame = 0x4fa417f2,
		};

	public:
		explicit InputBotInlineResult(QObject* parent = 0);
		virtual void read(MTProtoStream* mtstream);
		virtual void write(MTProtoStream* mtstream);
	protected:
		virtual void compileFlags();
	public:
		TLInt flags() const;
		void setFlags(TLInt flags);
		TLString id() const;
		void setId(TLString id);
		TLString type() const;
		void setType(TLString type);
		TLString title() const;
		void setTitle(TLString title);
		TLString description() const;
		void setDescription(TLString description);
		TLString url() const;
		void setUrl(TLString url);
		TLString thumbUrl() const;
		void setThumbUrl(TLString thumb_url);
		TLString contentUrl() const;
		void setContentUrl(TLString content_url);
		TLString contentType() const;
		void setContentType(TLString content_type);
		TLInt w() const;
		void setW(TLInt w);
		TLInt h() const;
		void setH(TLInt h);
		TLInt duration() const;
		void setDuration(TLInt duration);
		InputBotInlineMessage* sendMessage() const;
		void setSendMessage(InputBotInlineMessage* send_message);
		InputPhoto* photo() const;
		void setPhoto(InputPhoto* photo);
		InputDocument* document() const;
		void setDocument(InputDocument* document);
		TLString shortName() const;
		void setShortName(TLString short_name);

	signals:
		void flagsChanged();
		void idChanged();
		void typeChanged();
		void titleChanged();
		void descriptionChanged();
		void urlChanged();
		void thumbUrlChanged();
		void contentUrlChanged();
		void contentTypeChanged();
		void wChanged();
		void hChanged();
		void durationChanged();
		void sendMessageChanged();
		void photoChanged();
		void documentChanged();
		void shortNameChanged();

	private:
		TLInt _flags;
		TLString _id;
		TLString _type;
		TLString _title;
		TLString _description;
		TLString _url;
		TLString _thumb_url;
		TLString _content_url;
		TLString _content_type;
		TLInt _w;
		TLInt _h;
		TLInt _duration;
		InputBotInlineMessage* _send_message;
		InputPhoto* _photo;
		InputDocument* _document;
		TLString _short_name;

};

#endif // INPUTBOTINLINERESULT_H
