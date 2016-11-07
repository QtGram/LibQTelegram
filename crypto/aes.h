#ifndef AES_H
#define AES_H

#include <openssl/aes.h>
#include <QtCore>
#include "../types/basic.h"

class Aes
{
    private:
        Aes();

    public:
        static void calculateAesKeys(const TLBytes &authorizationkey, const TLBytes &msgkey, bool in, TLBytes &aeskey, TLBytes &aesiv);
        static QByteArray encrypt(const TLBytes &data, const TLBytes &key, const TLBytes &iv);
        static QByteArray decrypt(const TLBytes &data, const TLBytes &key, const TLBytes &iv);
};

#endif // AES_H
