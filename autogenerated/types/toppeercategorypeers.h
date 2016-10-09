#ifndef TOPPEERCATEGORYPEERS_H
#define TOPPEERCATEGORYPEERS_H

// Generated by APIGenerator 1.0
// DO NOT EDIT!!!

#include "../../types/telegramobject.h"
#include "toppeer.h"
#include "../../types/basic.h"
#include "toppeercategory.h"

class TopPeerCategoryPeers: public TelegramObject
{
	Q_OBJECT

	Q_PROPERTY(TopPeerCategory* category READ category WRITE setCategory NOTIFY categoryChanged)
	Q_PROPERTY(TLInt count READ count WRITE setCount NOTIFY countChanged)
	Q_PROPERTY(TLVector<TopPeer*> peers READ peers WRITE setPeers NOTIFY peersChanged)

	Q_ENUMS(Constructors)

	public:
		enum Constructors {
			ctorTopPeerCategoryPeers = 0xfb834291,
		};

	public:
		explicit TopPeerCategoryPeers(QObject* parent = 0);
		virtual void read(MTProtoStream* mtstream);
		virtual void write(MTProtoStream* mtstream);
	protected:
		virtual void compileFlags();
	public:
		TopPeerCategory* category() const;
		void setCategory(TopPeerCategory* category);
		TLInt count() const;
		void setCount(TLInt count);
		const TLVector<TopPeer*>& peers() const;
		void setPeers(const TLVector<TopPeer*>& peers);

	signals:
		void categoryChanged();
		void countChanged();
		void peersChanged();

	private:
		TopPeerCategory* _category;
		TLInt _count;
		TLVector<TopPeer*> _peers;

};

#endif // TOPPEERCATEGORYPEERS_H
