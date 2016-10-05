#include "rsa.h"
#include <QDebug>

Rsa::Rsa()
{

}

bool Rsa::solvePQ(TLLong pq, TLInt &p, TLInt &q)
{
    TLInt fact1 = Math::primeNumber(pq);

    if(fact1 == 1)
        return false;

    TLInt fact2 = pq / fact1;
    p = qMin(fact1, fact2);
    q = qMax(fact1, fact2);
    return true;
}

QByteArray Rsa::encrypt(const QByteArray &cleandata, const QString& publickey)
{
    FILE* fp = std::fopen(publickey.toUtf8().constData(), "r");

    if(!fp)
    {
        qDebug() << Q_FUNC_INFO << "Cannot read" << publickey;
        return QByteArray();
    }

    RSA *rsa = RSA_new();
    BN_CTX* bnctx = BN_CTX_new();

    if(!PEM_read_RSAPublicKey(fp, &rsa, NULL, NULL))
    {
        qDebug() << Q_FUNC_INFO << "Invalid public key:" << publickey;
        std::fclose(fp);
        return QByteArray();
    }

    int bits = BN_num_bits(rsa->n);
    Q_ASSERT(bits >= 2041 && bits <= 2048);

    QByteArray rsadata;
    rsadata.fill(0, RSA_size(rsa));

    BIGNUM *bncleandata = BN_new(), *bnresult = BN_new();
    BN_bin2bn(reinterpret_cast<const uchar*>(cleandata.constData()), cleandata.length(), bncleandata);
    Q_ASSERT(BN_mod_exp(bnresult, bncleandata, rsa->e, rsa->n, bnctx) == 1);

    unsigned int l = 256 - BN_num_bytes(bnresult);
    Q_ASSERT(l <= 256);

    BN_bn2bin(bnresult, reinterpret_cast<uchar*>(rsadata.data()));

    BN_CTX_free(bnctx);
    BN_free(bnresult);
    BN_free(bncleandata);
    std::fclose(fp);
    RSA_free(rsa);
    return rsadata;
}
