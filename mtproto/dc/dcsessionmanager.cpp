#include "dcsessionmanager.h"

DCSessionManager* DCSessionManager::_sessionmanager = NULL;

DCSessionManager::DCSessionManager(QObject *parent): QObject(parent), _mainsession(NULL)
{
}

DC *DCSessionManager::createDC(const QString &host, qint16 port, int id)
{
    if(this->_dclist.contains(id))
        return this->_dclist[id];

    DCConfig& dcconfig = GET_DC_CONFIG_FROM_DCID(id);
    dcconfig.setHost(host);
    dcconfig.setPort(port);

    DC* dc = new DC(host, port, id, this);
    connect(dc, &DC::authorizationReply, this, &DCSessionManager::onAuthorizationReply);
    connect(dc, &DC::migrateDC, this, &DCSessionManager::onMigrateDC);

    this->_dclist[id] = dc;
    return dc;
}

DC *DCSessionManager::createDC(int id)
{
    DCConfig& dcconfig = GET_DC_CONFIG_FROM_DCID(id);
    return this->createDC(dcconfig.host(), dcconfig.port(), id);
}

void DCSessionManager::doAuthorization(DCSession *dcsession)
{
    if(this->_dcauthorizations.contains(dcsession->dc())) // Do not authorize the same DC multiple times
        return;

    DCAuthorization* dcauthorization = new DCAuthorization(dcsession, this);

    connect(dcauthorization, &DCAuthorization::authorized, this, &DCSessionManager::onAuthorized);

    this->_dcauthorizations[dcsession->dc()] = dcauthorization;
    dcauthorization->authorize();
}

void DCSessionManager::initSession(DCSession *dcsession)
{
    DC* dc = dcsession->dc();
    Q_ASSERT(dc != NULL);

    DCConfig& dcconfig = GET_DC_CONFIG_FROM_DC(dc);

    if(dcconfig.authorization() < DCConfig::Authorized)
        this->doAuthorization(this->_mainsession);
}

void DCSessionManager::closeSession(DCSession *dcsession)
{
    DC* dc = dcsession->dc();
    Q_ASSERT(dc != NULL);

    if(dcsession->ownedDc())
        dc->close();

    dcsession->deleteLater();
}

DCSessionManager *DCSessionManager::instance()
{
    if(!DCSessionManager::_sessionmanager)
        DCSessionManager::_sessionmanager = new DCSessionManager();

    return DCSessionManager::_sessionmanager;
}

DCSession *DCSessionManager::mainSession() const
{
    return this->_mainsession;
}

DCSession *DCSessionManager::createMainSession(const DCConfig &dcconfig)
{
    return this->createMainSession(dcconfig.host(), dcconfig.port(), dcconfig.id());
}

DCSession* DCSessionManager::createMainSession(const QString &host, qint16 port, int dcid)
{
    DCSession* oldsession = this->_mainsession;
    DC* dc = this->createDC(host, port, dcid);

    this->_mainsession = new DCSession(dc, this);
    this->_mainsession->setOwnedDC(true);

    if(oldsession)
    {
        dc->takeRequests(this->_mainsession->sessionId(), this->_mainsession->lastMsgId(), oldsession->dc());
        this->closeSession(oldsession);
    }

    this->initSession(this->_mainsession);
    return this->_mainsession;
}

DCSession *DCSessionManager::createMainSession(int dcid)
{
    DCConfig& dcconfig = GET_DC_CONFIG_FROM_DCID(dcid);
    return this->createMainSession(dcconfig.host(), dcconfig.port(), dcid);
}

DCSession *DCSessionManager::createSession(int dcid)
{
    DC* dc = this->createDC(dcid);
    DCSession* dcsession = new DCSession(dc, this);

    this->initSession(dcsession);
    return dcsession;
}

void DCSessionManager::onAuthorized(DC* dc)
{
    Q_ASSERT(this->_dcauthorizations.contains(dc));
    TELEGRAM_CONFIG_SAVE;

    DCAuthorization* dcauthorization = this->_dcauthorizations.take(dc);
    dcauthorization->deleteLater();

    dc->sendPendingRequests();
}

void DCSessionManager::onAuthorizationReply(MTProtoReply *mtreply)
{
    DC* dc = qobject_cast<DC*>(this->sender());

    Q_ASSERT(this->_dcauthorizations.contains(dc));

    DCAuthorization* dcauthorization = this->_dcauthorizations[dc];
    dcauthorization->authorizeReply(mtreply);
}

void DCSessionManager::onMigrateDC(int fromdcid, int todcid)
{
    qDebug().noquote() << "DC" << fromdcid << "->" << todcid;
    this->createMainSession(todcid);
}
