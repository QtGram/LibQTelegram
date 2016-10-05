#ifndef RSA_H
#define RSA_H

#include <QByteArray>
#include <openssl/pem.h>
#include <openssl/rsa.h>
#include <openssl/bn.h>
#include <openssl/err.h>
#include "../types/basic.h"
#include "math.h"

class Rsa
{
    private:
        Rsa();

    public:
        static bool solvePQ(TLLong pq, TLInt& p, TLInt& q);
        static QByteArray encrypt(const QByteArray& cleandata, const QString &publickey);
};

#endif // RSA_H
