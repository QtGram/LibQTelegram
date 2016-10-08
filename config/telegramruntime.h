#ifndef TELEGRAMRUNTIME_H
#define TELEGRAMRUNTIME_H

#define TELEGRAM_RUNTIME TelegramRuntime::runtime()

#include <QObject>
#include <QHash>
#include <QList>
#include "../types/basic.h"
#include "../types/telegramhelper.h"
#include "../objects/objects.h"

class TelegramRuntime : public QObject
{
    Q_OBJECT

    private:
        explicit TelegramRuntime(QObject *parent = 0);

    public:
        static TelegramRuntime* runtime();
        QList<DialogObject *> dialogList() const;

    private slots:
        void onNewMessage(Message *message);
        void onUpdate(Update* update);

    private:
        void updateDialogs();
        DialogObject* updateDialog(Dialog* dialog);
        MessageObject* createMessageObject(Message* message);

    private:
        template<typename TG, typename RT> RT* runtimeObject(QHash<TLInt, RT*>& container, TG* tg);

    signals:
        void dialogsChanged();

    private:
        QHash<TLInt, DialogObject*> _dialogs;
        QHash<TLInt, MessageObject*> _messages;

    private:
        static TelegramRuntime* _instance;
};

template<typename TG, typename RT> RT* TelegramRuntime::runtimeObject(QHash<TLInt, RT*>& container, TG* tg)
{
    RT* rt = NULL;
    TLInt id = TelegramHelper::identifier(tg);

    if(!container.contains(id))
    {
        rt = new RT(this);
        container[id] = rt;
    }
    else
        rt = container[id];

    return rt;
}

#endif // TELEGRAMRUNTIME_H
