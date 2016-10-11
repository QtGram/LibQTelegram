#include "dcauthorization.h"
#include "../../types/byteconverter.h"
#include "../../config/telegramconfig.h"
#include "../../config/dcconfig.h"
#include "../../crypto/math.h"
#include "../../crypto/sha1.h"
#include "../../crypto/aes.h"
#include "../../crypto/rsa.h"

const int DCAuthorization::ENCRYPTED_PQINNERDATA_LENGTH = 255;

DCAuthorization::DCAuthorization(DCSession *dcsession, QObject *parent) : QObject(parent), _dcsession(dcsession), _respq(NULL), _retryid(0)
{
    connect(dcsession->dc(), &DC::configurationReceived, this, &DCAuthorization::onConfigurationReceived);
}

DCSession *DCAuthorization::dcSession() const
{
    return this->_dcsession;
}

void DCAuthorization::authorizeReply(MTProtoReply *mtreply)
{
    TLConstructor ctor = mtreply->constructorId();
    MTProtoStream mtstream(mtreply->body());

    if(ctor == TLTypes::ResPQ)
        this->onPQReceived(&mtstream);
    else if(ctor == TLTypes::ServerDHParamsFail)
        this->onServerDHParamsFailReceived(&mtstream);
    else if(ctor == TLTypes::ServerDHParamsOk)
        this->onServerDHParamsOkReceived(&mtstream);
    else if(ctor == TLTypes::DhGenFail)
        this->onServerDhGenFail(&mtstream);
    else if(ctor == TLTypes::DhGenRetry)
        this->onServerDhGenRetry(&mtstream);
    else if(ctor == TLTypes::DhGenOk)
        this->onServerDhGenOk(&mtstream);
}

void DCAuthorization::authorize()
{
    DCConfig& dcconfig = DCConfig_fromSession(this->_dcsession);

    if(dcconfig.authorization() == DCConfig::NotAuthorized)
        this->handleNotAuthorized();
    else if(dcconfig.authorization() == DCConfig::PQReceived)
        this->handlePQReceived();
    else if(dcconfig.authorization() == DCConfig::ServerDHParamsFailReceived)
        this->handleDHParamsFail();
    else if(dcconfig.authorization() == DCConfig::ServerDHParamsOkReceived)
        this->handleDHParamsOk();
    else if(dcconfig.authorization() == DCConfig::Authorized)
        this->handleAuthorized();
    else
        Q_ASSERT(false);
}

void DCAuthorization::importAuthorization(DCSession* fromsession)
{
    Q_ASSERT(DCConfig_isLoggedIn);
    Q_ASSERT(this->_dcsession->dc());

    MTProtoRequest* req = TelegramAPI::authExportAuthorization(fromsession, this->_dcsession->dc()->id());
    connect(req, &MTProtoRequest::replied, this, &DCAuthorization::onAuthorizationExported);
}

void DCAuthorization::encryptPQInnerData(PQInnerData *pqinnerdata, TLBytes &encinnerdata)
{
    MTProtoStream mtstream;
    pqinnerdata->write(&mtstream);

    TLBytes datawithhash = Sha1::hash(mtstream.data());
    datawithhash.append(mtstream.data());

    TLInt datalen = datawithhash.length(), randlen = DCAuthorization::ENCRYPTED_PQINNERDATA_LENGTH - datalen;
    datawithhash.resize(DCAuthorization::ENCRYPTED_PQINNERDATA_LENGTH);

    Math::randomize(datawithhash, datalen, randlen);
    encinnerdata = Rsa::encrypt(datawithhash, TelegramConfig::config()->publicKey());
}

void DCAuthorization::encryptClientDHInnerData(ClientDHInnerData *clientdhinnerdata, const TLBytes& tmpaeskey, const TLBytes& tmpaesiv, TLBytes &encclientdhinnerdata)
{
    MTProtoStream mtstream;
    clientdhinnerdata->write(&mtstream);

    TLBytes datawithhash = Sha1::hash(mtstream.data());
    datawithhash.append(mtstream.data());

    TLInt datalen = datawithhash.length(), randlen = Math::padRequired(datawithhash.length(), 16);
    datawithhash.resize(datawithhash.length() + randlen);
    Math::randomize(datawithhash, datalen, randlen);

    encclientdhinnerdata = Aes::encrypt(datawithhash, tmpaeskey, tmpaesiv);
}

void DCAuthorization::handleNotAuthorized()
{
    Math::randomize(&this->_nonce);
    MTProtoAPI::reqPq(this->_dcsession, this->_nonce);
}

void DCAuthorization::handlePQReceived()
{
    TLInt p = 0, q = 0;
    TLLong pq = ByteConverter::integer<TLLong>(this->_respq->pq());

    if(!Rsa::solvePQ(pq, p, q))
    {
        qFatal("Unable to solve PQ\n");
        return;
    }

    TLBytes bp = ByteConverter::bytes(p), bq = ByteConverter::bytes(q);
    Math::randomize(&this->_newnonce);

    PQInnerData pqinnerdata;
    pqinnerdata.setPq(this->_respq->pq());
    pqinnerdata.setP(bp);
    pqinnerdata.setQ(bq);
    pqinnerdata.setNonce(this->_nonce);
    pqinnerdata.setServerNonce(this->_respq->serverNonce());
    pqinnerdata.setNewNonce(this->_newnonce);

    TLBytes encinnerdata;
    this->encryptPQInnerData(&pqinnerdata, encinnerdata);

    MTProtoAPI::reqDHParams(this->_dcsession, this->_nonce,
                            this->_respq->serverNonce(), bp, bq,
                            this->_respq->serverPublicKeyFingerprints().first(),
                            encinnerdata);
}

void DCAuthorization::handleDHParamsFail()
{
    qDebug() << "DHPARAMS_FAIL not implemented";
}

void DCAuthorization::handleDHParamsOk()
{
    ClientDHInnerData clientdhinnerdata;
    clientdhinnerdata.setNonce(this->_respq->nonce());
    clientdhinnerdata.setServerNonce(this->_respq->serverNonce());
    clientdhinnerdata.setRetryId(this->_retryid);
    clientdhinnerdata.setGB(this->_g_b);

    TLBytes encclientdhinnerdata;
    this->encryptClientDHInnerData(&clientdhinnerdata, this->_tmpaeskey, this->_tmpaesiv, encclientdhinnerdata);
    MTProtoAPI::setClientDHParams(this->_dcsession, this->_respq->nonce(), this->_respq->serverNonce(), encclientdhinnerdata);
}

void DCAuthorization::handleAuthorized()
{
    TelegramAPI::helpGetConfig(this->_dcsession);
}

void DCAuthorization::onPQReceived(MTProtoStream *mtstream)
{
    DCConfig& dcconfig = DCConfig_fromSession(this->_dcsession);
    dcconfig.setAuthorization(DCConfig::PQReceived);

    this->_respq = new ResPQ(this);
    this->_respq->read(mtstream);

    this->authorize();
}

void DCAuthorization::onServerDHParamsOkReceived(MTProtoStream *mtstream)
{
    DCConfig& dcconfig = DCConfig_fromSession(this->_dcsession);
    dcconfig.setAuthorization(DCConfig::ServerDHParamsOkReceived);

    ServerDHParams serverdhparams;
    serverdhparams.read(mtstream);

    TLBytes newnonce = ByteConverter::serialize(this->_newnonce);
    TLBytes servernonce = ByteConverter::serialize(this->_respq->serverNonce());

    this->_tmpaeskey = Sha1::hash(newnonce + servernonce) + Sha1::hash(servernonce + newnonce).left(12);
    this->_tmpaesiv = Sha1::hash(servernonce + newnonce).mid(12, 8) + Sha1::hash(newnonce + newnonce) + newnonce.left(4);

    TLBytes answer = Aes::decrypt(serverdhparams.encryptedAnswer(), this->_tmpaeskey, this->_tmpaesiv);

    // Sanity checks
    TLBytes sha1answer = answer.left(Sha1::BYTES_COUNT);
    answer = answer.mid(Sha1::BYTES_COUNT, 564); // Strip SHA1 part
    Q_ASSERT(sha1answer == Sha1::hash(answer));

    MTProtoStream mtstreamanswer(answer);
    ServerDHInnerData serverdhinnerdata;
    serverdhinnerdata.read(&mtstreamanswer);

    // More sanity checks
    Q_ASSERT_X((serverdhinnerdata.g() >= 2) && (serverdhinnerdata.g() <= 7), "ServerDHParamsOK", "Received 'g' number is out of acceptable range [2-7]");
    Q_ASSERT_X(serverdhinnerdata.dhPrime().length() == (2048 / 8), "ServerDHParamsOK", "Received dhPrime number length is not correct");
    Q_ASSERT_X(serverdhinnerdata.dhPrime()[0] & 128, "ServerDHParamsOK", "Received dhPrime is too small");

    TLBytes b;
    b.fill(0, 256);

    this->_g_b.clear();
    this->_g_b.fill(0, sizeof(serverdhinnerdata.g()));

    Math::randomize(b);
    qToBigEndian(serverdhinnerdata.g(), reinterpret_cast<uchar*>(this->_g_b.data()));
    this->_g_b = Math::modExp(this->_g_b, serverdhinnerdata.dhPrime(), b);

    dcconfig.setDeltaTime(CurrentDeltaTime(serverdhinnerdata.serverTime()));
    dcconfig.setAuthorizationKey(Math::modExp(serverdhinnerdata.gA(), serverdhinnerdata.dhPrime(), b));

    this->authorize();
}

void DCAuthorization::onServerDHParamsFailReceived(MTProtoStream *mtstream)
{
    DCConfig& dcconfig = DCConfig_fromSession(this->_dcsession);
    dcconfig.setAuthorization(DCConfig::ServerDHParamsFailReceived);
}

void DCAuthorization::onServerDhGenFail(MTProtoStream *mtstream)
{
    SetClientDHParamsAnswer clientdhparamsanswer;
    clientdhparamsanswer.read(mtstream);

    Q_ASSERT(clientdhparamsanswer.constructorId() == TLTypes::DhGenFail);
}

void DCAuthorization::onServerDhGenRetry(MTProtoStream *mtstream)
{
    SetClientDHParamsAnswer clientdhparamsanswer;
    clientdhparamsanswer.read(mtstream);

    Q_ASSERT(clientdhparamsanswer.constructorId() == TLTypes::DhGenRetry);
}

void DCAuthorization::onServerDhGenOk(MTProtoStream *mtstream)
{
    DCConfig& dcconfig = DCConfig_fromSession(this->_dcsession);

    SetClientDHParamsAnswer clientdhparamsanswer;
    clientdhparamsanswer.read(mtstream);

    Q_ASSERT(clientdhparamsanswer.constructorId() == TLTypes::DhGenOk);

    TLBytes expectedhashdata = ByteConverter::serialize(this->_newnonce);
    TLBytes newnoncehash1 = ByteConverter::serialize(clientdhparamsanswer.newNonceHash1());

    expectedhashdata.append(static_cast<char>(1));
    expectedhashdata.append(dcconfig.authorizationKeyAuxHash());

    Q_ASSERT_X(Sha1::hash(expectedhashdata).mid(4) == newnoncehash1, "ServerDhGenOk", "Server (newnonce + auth_key_aux_hash) hash is not correct.");

    dcconfig.setServerSalt(this->_newnonce.lo.lo ^ clientdhparamsanswer.serverNonce().lo);
    dcconfig.setAuthorization(DCConfig::Authorized);
    this->authorize();
}

void DCAuthorization::onConfigurationReceived(Config *config)
{
    foreach(const DcOption* dcoption, config->dcOptions())
    {
        if(dcoption->isMediaOnly()) // NOTE: "Media Only" DCs needs investigation
            continue;

        DCConfig& dcconfig = TelegramConfig::config()->setDcConfig(dcoption->id(), dcoption->isIpv6());

        if(!dcconfig.host().isEmpty() && (dcconfig.host() != dcoption->ipAddress())) // DC Configuration has changed
            dcconfig.reset();

        dcconfig.setHost(dcoption->ipAddress());
        dcconfig.setPort(dcoption->port());
        dcconfig.setId(dcoption->id());
    }

    TelegramConfig_save;
    emit authorized(this->_dcsession->dc());
}

void DCAuthorization::onAuthorizationExported(MTProtoReply *mtreply)
{
    AuthExportedAuthorization exportedauthorization;
    exportedauthorization.read(mtreply);

    MTProtoRequest* req = TelegramAPI::authImportAuthorization(this->_dcsession, exportedauthorization.id(), exportedauthorization.bytes());
    connect(req, &MTProtoRequest::replied, this, &DCAuthorization::onAuthorizationImported);
}

void DCAuthorization::onAuthorizationImported(MTProtoReply *mtreply)
{
    Q_UNUSED(mtreply);

    DCConfig& dcconfig = DCConfig_fromSession(this->_dcsession);
    dcconfig.setAuthorization(DCConfig::Signed);

    TelegramConfig_save;
    emit authorizationImported(this->_dcsession->dc());
}
