#include "dcsessionmanager.h"

DCSessionManager* DCSessionManager::_sessionmanager = NULL;

DCSessionManager::DCSessionManager(QObject *parent): QObject(parent), _mainsession(NULL)
{
}

DC *DCSessionManager::createDC(const QString &host, qint16 port, int id)
{
    if(this->_dclist.contains(id))
        return this->_dclist[id];

    qDebug() << "DC" << id << "created";

    DCConfig& dcconfig = DCConfig_fromDcId(id);
    dcconfig.setHost(host);
    dcconfig.setPort(port);

    DC* dc = new DC(host, port, id, this);
    connect(dc, &DC::disconnected, this, &DCSessionManager::onDCDisconnected);
    connect(dc, &DC::authorizationReply, this, &DCSessionManager::onAuthorizationReply);
    connect(dc, &DC::migrateDC, this, &DCSessionManager::onMigrateDC);
    connect(dc, &DC::floodWait, this, &DCSessionManager::floodWait);

    this->_dclist[id] = dc;
    return dc;
}

DC *DCSessionManager::createDC(int id)
{
    DCConfig& dcconfig = DCConfig_fromDcId(id);
    return this->createDC(dcconfig.host(), dcconfig.port(), id);
}

void DCSessionManager::doAuthorization(DCSession *dcsession)
{
    if(this->_dcauthorizations.contains(dcsession->dc())) // Do not authorize the same DC multiple times
        return;

    DCAuthorization* dcauthorization = new DCAuthorization(dcsession, this);
    connect(dcauthorization, &DCAuthorization::authorized, this, &DCSessionManager::onAuthorized);
    connect(dcauthorization, &DCAuthorization::authorizationImported, this, &DCSessionManager::onAuthorizationImported);

    this->_dcauthorizations[dcsession->dc()] = dcauthorization;
    dcauthorization->authorize();
}

void DCSessionManager::doSessionReady(DCSession *dcsession)
{
    DC* dc = dcsession->dc();
    Q_ASSERT(dc != NULL);

    MTProtoRequest* keptreq = dc->giveRequest();

    if(keptreq)
    {
        keptreq->setSessionId(dcsession->sessionId());
        dc->send(keptreq);
    }

    emit sessionReady(dcsession);
    emit dcsession->ready();
}

void DCSessionManager::initializeSession(DCSession *dcsession)
{
    DC* dc = dcsession->dc();
    Q_ASSERT(dc != NULL);

    if(dc->state() != DC::ConnectedState)
    {
        if(dc->state() == DC::UnconnectedState)
            dc->connectToDC();

        return;
    }

    DCConfig& dcconfig = DCConfig_fromDc(dc);

    if(dcconfig.authorization() < DCConfig::Authorized)
        this->doAuthorization(dcsession);
    else
        this->doSessionReady(dcsession);
}

void DCSessionManager::closeSession(DCSession *dcsession)
{
    DC* dc = dcsession->dc();
    Q_ASSERT(dc != NULL);

    if(dcsession->ownedDc())
    {
        qDebug("DC %d closed", dc->id());
        dc->close();
        this->_dclist.remove(dc->id());
    }

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

    connect(this->_mainsession, &DCSession::unauthorized, this, &DCSessionManager::initializeSession, Qt::UniqueConnection);

    if(oldsession)
    {
        DCConfig& olddcconfig = DCConfig_fromSession(oldsession);

        if(olddcconfig.authorization() == DCConfig::Signed) // Get the last request only if the previous DC was signed
            dc->keepRequest(oldsession->dc()->lastRequest());

        this->closeSession(oldsession);
    }

    DCConfig_setMainDc(dcid);
    return this->_mainsession;
}

DCSession *DCSessionManager::createMainSession(int dcid)
{
    DCConfig& dcconfig = DCConfig_fromDcId(dcid);
    return this->createMainSession(dcconfig.host(), dcconfig.port(), dcid);
}

DCSession *DCSessionManager::createSession(int dcid)
{
    DC* dc = this->createDC(dcid);
    DCSession* dcsession = new DCSession(dc, this);

    connect(dcsession, &DCSession::unauthorized, this, &DCSessionManager::initializeSession, Qt::UniqueConnection);
    return dcsession;
}

void DCSessionManager::onAuthorized(DC* dc)
{
    Q_ASSERT(this->_dcauthorizations.contains(dc));

    DCAuthorization* dcauthorization = this->_dcauthorizations[dc];

    if(DCConfig_isLoggedIn) // Wait, there is another step
    {
        dcauthorization->importAuthorization(this->_mainsession);
        return;
    }

    DCSession* dcsession = dcauthorization->dcSession();

    this->_dcauthorizations.remove(dc);
    dcauthorization->deleteLater();
    this->doSessionReady(dcsession);
}

void DCSessionManager::onAuthorizationImported(DC *dc)
{
    Q_ASSERT(this->_dcauthorizations.contains(dc));

    DCAuthorization* dcauthorization = this->_dcauthorizations.take(dc);
    DCSession* dcsession = dcauthorization->dcSession();

    dcauthorization->deleteLater();
    this->doSessionReady(dcsession);
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
    qDebug("DC %d -> %d", fromdcid, todcid);

    DCSession* session = this->createMainSession(todcid);
    this->initializeSession(session);
}

void DCSessionManager::onDCDisconnected()
{
    DC* dc = qobject_cast<DC*>(this->sender());

    if(!this->_dcauthorizations.contains(dc))
        return;

    DCAuthorization* dcauthorization = this->_dcauthorizations.take(dc);
    dcauthorization->deleteLater();
}
