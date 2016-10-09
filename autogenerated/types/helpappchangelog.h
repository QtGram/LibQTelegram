#ifndef HELPAPPCHANGELOG_H
#define HELPAPPCHANGELOG_H

// Generated by APIGenerator 1.0
// DO NOT EDIT!!!

#include "../../types/telegramobject.h"
#include "../../types/basic.h"

class HelpAppChangelog: public TelegramObject
{
	Q_OBJECT

	Q_PROPERTY(TLString text READ text WRITE setText NOTIFY textChanged)

	Q_ENUMS(Constructors)

	public:
		enum Constructors {
			ctorHelpAppChangelogEmpty = 0xaf7e0394,
			ctorHelpAppChangelog = 0x4668e6bd,
		};

	public:
		explicit HelpAppChangelog(QObject* parent = 0);
		virtual void read(MTProtoStream* mtstream);
		virtual void write(MTProtoStream* mtstream);
	protected:
		virtual void compileFlags();
	public:
		TLString text() const;
		void setText(TLString text);

	signals:
		void textChanged();

	private:
		TLString _text;

};

#endif // HELPAPPCHANGELOG_H
