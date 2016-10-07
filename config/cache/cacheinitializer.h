#ifndef CACHEINITIALIZER_H
#define CACHEINITIALIZER_H

#include <QObject>
#include "../../mtproto/mtprotoreply.h"

class CacheInitializer : public QObject
{
    Q_OBJECT

    private:
        enum State {
            First = 0,
            RequestContacts = First,
            RequestDialogs,
            Last = RequestDialogs,
        };

    public:
        explicit CacheInitializer(QObject *parent = 0);
        void initialize();

    private:
        void requestContacts();
        void requestDialogs();

    private:
        void onRequestContactsReplied(MTProtoReply* mtreply);
        void onRequestDialogsReplied(MTProtoReply* mtreply);

    signals:
        void initialized();

    private:
        int _state;
};

#endif // CACHEINITIALIZER_H
