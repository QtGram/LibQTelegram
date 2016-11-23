#include "dcsessionmanager.h"

DCSessionManager* DCSessionManager::_sessionmanager = NULL;

DCSessionManager::DCSessionManager(QObject *parent): QObject(parent), _mainsession(NULL)
{
}

DC *DCSessionManager::createDC(DCConfig *dcconfig, bool filedc)
{
    if(filedc && this->_filedclist.contains(dcconfig->id()))
        return this->_filedclist[dcconfig->id()];
    else if(!filedc && this->_dclist.contains(dcconfig->id()))
        return this->_dclist[dcconfig->id()];

    DC* dc = new DC(dcconfig, filedc, this);
    connect(dc, &DC::disconnected, this, &DCSessionManager::onDCDisconnected);
    connect(dc, &DC::authorizationReply, this, &DCSessionManager::onAuthorizationReply);
    connect(dc, &DC::migrateDC, this, &DCSessionManager::onMigrateDC);
    connect(dc, &DC::phoneCodeError, this, &DCSessionManager::phoneCodeError);
    connect(dc, &DC::floodLock, this, &DCSessionManager::floodLock);
    connect(dc, &DC::invalidPassword, this, &DCSessionManager::invalidPassword);
    connect(dc, &DC::sessionPasswordNeeded, this, &DCSessionManager::sessionPasswordNeeded);

    dc->log("created");

    if(filedc)
        this->_filedclist[dcconfig->id()] = dc;
    else
        this->_dclist[dcconfig->id()] = dc;

    return dc;
}

void DCSessionManager::doRestoreSessions(const QList<DC *> &dclist) const
{
    foreach(DC* dc, dclist)
        dc->reconnect();
}

void DCSessionManager::doAuthorization(DCSession *dcsession)
{
    if(this->_dcauthorizations.contains(SessionToDC(dcsession))) // Do not authorize the same DC multiple times
        return;

    DCConfig* dcconfig = SessionToDcConfig(dcsession);
    dcconfig->setAuthorization(DCConfig::NotAuthorized); // Reset undefined authorization state (if any)

    DCAuthorization* dcauthorization = new DCAuthorization(dcsession, this);
    connect(dcauthorization, &DCAuthorization::authorized, this, &DCSessionManager::onAuthorized);
    connect(dcauthorization, &DCAuthorization::authorizationImported, this, &DCSessionManager::onAuthorizationImported);

    this->_dcauthorizations[SessionToDC(dcsession)] = dcauthorization;
    dcauthorization->authorize();
}

void DCSessionManager::doSessionReady(DCSession *dcsession)
{
    DC* dc = SessionToDC(dcsession);
    Q_ASSERT(dc != NULL);

    MTProtoRequest* keptreq = dc->giveRequest();

    if(keptreq)
    {
        keptreq->setSessionId(dcsession->sessionId());
        dc->send(keptreq);
    }

    dc->log("Client Session Created (session_id: %llx)", dcsession->sessionId());
    dcsession->repeatRequests();

    emit sessionReady(dcsession);
    emit dcsession->ready();
}

void DCSessionManager::initializeSession(DCSession *dcsession)
{
    DC* dc = SessionToDC(dcsession);
    Q_ASSERT(dc != NULL);

    if(dc->state() != DC::ConnectedState)
    {
        if(dc->state() == DC::UnconnectedState)
            dc->connectToDC();

        return;
    }

    DCConfig* dcconfig = SessionToDcConfig(dcsession);

    if(dcconfig->authorization() < DCConfig::Authorized)
        this->doAuthorization(dcsession);
    else
        this->doSessionReady(dcsession);
}

void DCSessionManager::closeSession(DCSession *dcsession)
{
    DC* dc = SessionToDC(dcsession);
    Q_ASSERT(dc != NULL);

    dc->log("Client Session Destroyed (session_id: %llx)", dcsession->sessionId());

    if(dcsession->ownedDc())
    {
        dc->close();
        dc->log("closed");

        if(dc->fileDc())
            this->_filedclist.remove(dc->config()->id());
        else
            this->_dclist.remove(dc->config()->id());
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

DCSession *DCSessionManager::createMainSession(DCConfig *dcconfig)
{
    DCSession* oldsession = this->_mainsession;
    DC* dc = this->createDC(dcconfig, false);

    this->_mainsession = new DCSession(dc, this);
    this->_mainsession->setOwnedDC(true);

    connect(this->_mainsession, &DCSession::unauthorized, this, &DCSessionManager::initializeSession, Qt::UniqueConnection);

    if(oldsession)
    {
        DC* olddc = SessionToDC(this->_mainsession);

        if(olddc)
        {
            disconnect(olddc, &DC::connected, this, 0);
            disconnect(olddc, &DC::disconnected, this, 0);
            disconnect(olddc, &DC::timeout, this, 0);
        }

        DCConfig* olddcconfig = SessionToDcConfig(oldsession);

        if(olddcconfig->authorization() == DCConfig::Signed) // Get the last request only if the previous DC was signed
            dc->keepRequest(SessionToDC(oldsession)->lastRequest());

        this->closeSession(oldsession);
    }

    DCConfig_setMainConfig(dcconfig);
    connect(dc, &DC::connected, this, &DCSessionManager::mainSessionConnectedChanged);
    connect(dc, &DC::disconnected, this, &DCSessionManager::mainSessionConnectedChanged);
    connect(dc, &DC::timeout, this, &DCSessionManager::mainSessionTimeout);
    return this->_mainsession;
}

DCSession *DCSessionManager::createSession(DCConfig *dcconfig, bool filedc)
{
    DC* dc = this->createDC(dcconfig, filedc);
    DCSession* dcsession = new DCSession(dc, this);

    connect(dcsession, &DCSession::unauthorized, this, &DCSessionManager::initializeSession, Qt::UniqueConnection);
    return dcsession;
}

void DCSessionManager::restoreSessions() const
{
    QList<DC*> dclist = this->_dclist.values();
    this->doRestoreSessions(dclist);

    dclist = this->_filedclist.values();
    this->doRestoreSessions(dclist);
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

void DCSessionManager::onMigrateDC(DCConfig* fromdcconfig, int todcid)
{
    qDebug("DC %d -> %d", fromdcconfig->dcid(), todcid);

    DCSession* session = this->createMainSession(DCConfig_migrateConfig(fromdcconfig, todcid));
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
