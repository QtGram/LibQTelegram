#ifndef KEYBOARDBUTTONROW_H
#define KEYBOARDBUTTONROW_H

// Generated by APIGenerator 1.0
// DO NOT EDIT!!!

#include "../../types/basic.h"
#include "../../types/telegramobject.h"
#include "keyboardbutton.h"

class KeyboardButtonRow: public TelegramObject
{
	Q_OBJECT

	Q_PROPERTY(TLVector<KeyboardButton*> buttons READ buttons WRITE setButtons NOTIFY buttonsChanged)

	Q_ENUMS(Constructors)

	public:
		enum Constructors {
			CtorKeyboardButtonRow = 0x77608b83,
		};

	public:
		explicit KeyboardButtonRow(QObject* parent = 0);
		virtual void read(MTProtoStream* mtstream);
		virtual void write(MTProtoStream* mtstream);
	protected:
		virtual void compileFlags();
	public:
		const TLVector<KeyboardButton*>& buttons() const;
		void setButtons(const TLVector<KeyboardButton*>& buttons);

	signals:
		void buttonsChanged();

	private:
		TLVector<KeyboardButton*> _buttons;

};

#endif // KEYBOARDBUTTONROW_H
