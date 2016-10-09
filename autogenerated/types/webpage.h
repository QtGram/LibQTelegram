#ifndef WEBPAGE_H
#define WEBPAGE_H

// Generated by APIGenerator 1.0
// DO NOT EDIT!!!

#include "photo.h"
#include "../../types/telegramobject.h"
#include "../../types/basic.h"
#include "document.h"

class WebPage: public TelegramObject
{
	Q_OBJECT

	Q_PROPERTY(TLLong id READ id WRITE setId NOTIFY idChanged)
	Q_PROPERTY(TLInt date READ date WRITE setDate NOTIFY dateChanged)
	Q_PROPERTY(TLInt flags READ flags WRITE setFlags NOTIFY flagsChanged)
	Q_PROPERTY(TLString url READ url WRITE setUrl NOTIFY urlChanged)
	Q_PROPERTY(TLString displayUrl READ displayUrl WRITE setDisplayUrl NOTIFY displayUrlChanged)
	Q_PROPERTY(TLString type READ type WRITE setType NOTIFY typeChanged)
	Q_PROPERTY(TLString siteName READ siteName WRITE setSiteName NOTIFY siteNameChanged)
	Q_PROPERTY(TLString title READ title WRITE setTitle NOTIFY titleChanged)
	Q_PROPERTY(TLString description READ description WRITE setDescription NOTIFY descriptionChanged)
	Q_PROPERTY(Photo* photo READ photo WRITE setPhoto NOTIFY photoChanged)
	Q_PROPERTY(TLString embedUrl READ embedUrl WRITE setEmbedUrl NOTIFY embedUrlChanged)
	Q_PROPERTY(TLString embedType READ embedType WRITE setEmbedType NOTIFY embedTypeChanged)
	Q_PROPERTY(TLInt embedWidth READ embedWidth WRITE setEmbedWidth NOTIFY embedWidthChanged)
	Q_PROPERTY(TLInt embedHeight READ embedHeight WRITE setEmbedHeight NOTIFY embedHeightChanged)
	Q_PROPERTY(TLInt duration READ duration WRITE setDuration NOTIFY durationChanged)
	Q_PROPERTY(TLString author READ author WRITE setAuthor NOTIFY authorChanged)
	Q_PROPERTY(Document* document READ document WRITE setDocument NOTIFY documentChanged)

	Q_ENUMS(Constructors)

	public:
		enum Constructors {
			ctorWebPageEmpty = 0xeb1477e8,
			ctorWebPagePending = 0xc586da1c,
			ctorWebPage = 0xca820ed7,
		};

	public:
		explicit WebPage(QObject* parent = 0);
		virtual void read(MTProtoStream* mtstream);
		virtual void write(MTProtoStream* mtstream);
	protected:
		virtual void compileFlags();
	public:
		TLLong id() const;
		void setId(TLLong id);
		TLInt date() const;
		void setDate(TLInt date);
		TLInt flags() const;
		void setFlags(TLInt flags);
		TLString url() const;
		void setUrl(TLString url);
		TLString displayUrl() const;
		void setDisplayUrl(TLString display_url);
		TLString type() const;
		void setType(TLString type);
		TLString siteName() const;
		void setSiteName(TLString site_name);
		TLString title() const;
		void setTitle(TLString title);
		TLString description() const;
		void setDescription(TLString description);
		Photo* photo() const;
		void setPhoto(Photo* photo);
		TLString embedUrl() const;
		void setEmbedUrl(TLString embed_url);
		TLString embedType() const;
		void setEmbedType(TLString embed_type);
		TLInt embedWidth() const;
		void setEmbedWidth(TLInt embed_width);
		TLInt embedHeight() const;
		void setEmbedHeight(TLInt embed_height);
		TLInt duration() const;
		void setDuration(TLInt duration);
		TLString author() const;
		void setAuthor(TLString author);
		Document* document() const;
		void setDocument(Document* document);

	signals:
		void idChanged();
		void dateChanged();
		void flagsChanged();
		void urlChanged();
		void displayUrlChanged();
		void typeChanged();
		void siteNameChanged();
		void titleChanged();
		void descriptionChanged();
		void photoChanged();
		void embedUrlChanged();
		void embedTypeChanged();
		void embedWidthChanged();
		void embedHeightChanged();
		void durationChanged();
		void authorChanged();
		void documentChanged();

	private:
		TLLong _id;
		TLInt _date;
		TLInt _flags;
		TLString _url;
		TLString _display_url;
		TLString _type;
		TLString _site_name;
		TLString _title;
		TLString _description;
		Photo* _photo;
		TLString _embed_url;
		TLString _embed_type;
		TLInt _embed_width;
		TLInt _embed_height;
		TLInt _duration;
		TLString _author;
		Document* _document;

};

#endif // WEBPAGE_H
