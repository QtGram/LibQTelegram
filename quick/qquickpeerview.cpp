#include "qquickpeerview.h"
#include "../cache/telegramcache.h"

QQuickPeerView::QQuickPeerView(QQuickItem* parent): QQuickBaseItem(parent), _delegate(NULL), _peer(NULL), _size(0)
{
}

TelegramObject *QQuickPeerView::peer() const
{
    return this->_peer;
}

QQmlComponent *QQuickPeerView::delegate() const
{
    return this->_delegate;
}

QString QQuickPeerView::fallbackText() const
{
    return this->fallbackText(this->_peer);
}

int QQuickPeerView::size() const
{
    return this->_size;
}

void QQuickPeerView::setPeer(TelegramObject *peer)
{
    if(this->_peer == peer)
        return;

    this->_peer = peer;
    emit peerChanged();
}

void QQuickPeerView::setDelegate(QQmlComponent *component)
{
    if(this->_delegate == component)
        return;

    this->_delegate = component;
    emit delegateChanged();
}

void QQuickPeerView::setSize(int size)
{
    if(this->_size == size)
        return;

    this->_size = size;
    this->updateMetrics();
    emit sizeChanged();
}

TelegramObject *QQuickPeerView::findPeer(TelegramObject *peer)
{
    if(peer->constructorId() == TLTypes::Dialog)
    {
        Dialog* dialog = qobject_cast<Dialog*>(peer);
        TLInt dialogid = TelegramHelper::identifier(dialog);

        if(TelegramHelper::isChat(dialog) ||TelegramHelper::isChannel(dialog))
        {
            Chat* chat = TelegramCache_chat(dialogid);

            if(chat)
            {
                connect(chat, &Chat::photoChanged, this, &QQuickPeerView::updateView, Qt::UniqueConnection);
                return chat;
            }
        }
        else
        {
            User* user = TelegramCache_user(dialogid);

            if(user)
            {
                connect(user, &User::photoChanged, this, &QQuickPeerView::updateView, Qt::UniqueConnection);
                return user;
            }
        }
    }
    else if(peer->constructorId() == TLTypes::Message)
    {
        Message* message = qobject_cast<Message*>(peer);

        if(message->isPost() || !message->fromId())
            return peer;

        User* user = TelegramCache_user(message->fromId());
        connect(user, &User::photoChanged, this, &QQuickPeerView::updateView, Qt::UniqueConnection);

        if(user)
            return user;
    }

    return peer;
}

void QQuickPeerView::initialize()
{
    if(!this->_peer || !this->_delegate)
        return;

    this->updateView();
    this->createObject(this->_delegate);

    emit fallbackTextChanged();
}

void QQuickPeerView::updateView()
{
    this->createFileObject(this->findPeer(this->_peer));
}

void QQuickPeerView::updateMetrics()
{
    this->setWidth(this->_size);
    this->setHeight(this->_size);
}

QString QQuickPeerView::fallbackText(TelegramObject *telegramobj) const
{
    if(!telegramobj)
        return QString();

    if(telegramobj->constructorId() == TLTypes::Dialog)
        return this->fallbackText(qobject_cast<Dialog*>(telegramobj)->peer());

    if(telegramobj->constructorId() == TLTypes::Chat)
        return this->fallbackTitle(qobject_cast<Chat*>(telegramobj)->title());

    if(telegramobj->constructorId() == TLTypes::User)
        return this->fallbackTitle(TelegramHelper::fullName(qobject_cast<User*>(telegramobj)));

    if(telegramobj->constructorId() == TLTypes::PeerChannel)
    {
        Chat* chat = TelegramCache_chat(qobject_cast<Peer*>(telegramobj)->channelId());
        return this->fallbackText(chat);
    }

    if(telegramobj->constructorId() == TLTypes::PeerChat)
    {
        Chat* chat = TelegramCache_chat(qobject_cast<Peer*>(telegramobj)->chatId());
        return this->fallbackText(chat);
    }

    if(telegramobj->constructorId() == TLTypes::PeerUser)
    {
        User* user = TelegramCache_user(qobject_cast<Peer*>(telegramobj)->userId());
        return this->fallbackText(user);
    }

    return QString();
}

QString QQuickPeerView::fallbackTitle(const QString &title) const
{
    QStringList words = title.split(" ");

    if(words.length() >= 2)
        return words[0].mid(0, 1).toUpper() + words[1].mid(0, 1).toUpper();

    return words[0].mid(0, 1).toUpper();
}

void QQuickPeerView::componentComplete()
{
    QQuickBaseItem::componentComplete();

    if(this->_peer && this->_delegate)
        this->initialize();
}
