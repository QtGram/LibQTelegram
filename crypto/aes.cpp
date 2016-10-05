#include "aes.h"

Aes::Aes()
{
}

void Aes::calculateAesKeys(const TLBytes &authorizationkey, const TLBytes &msgkey, bool in, TLBytes &aeskey, TLBytes &aesiv)
{
    int x = in ? 8 : 0;

    TLBytes sha1a = Sha1::hash(msgkey + authorizationkey.mid(x, 32));
    TLBytes sha1b = Sha1::hash(authorizationkey.mid(32 + x, 16) + msgkey + authorizationkey.mid(48 + x, 16));
    TLBytes sha1c = Sha1::hash(authorizationkey.mid(64 + x, 32) + msgkey);
    TLBytes sha1d = Sha1::hash(msgkey + authorizationkey.mid(96 + x, 32));

    aeskey = sha1a.mid(0, 8) + sha1b.mid(8, 12) + sha1c.mid(4, 12);
    aesiv = sha1a.mid(8, 12) + sha1b.mid(0, 8) + sha1c.mid(16, 4) + sha1d.mid(0, 8);
}

QByteArray Aes::encrypt(const TLBytes &data, const TLBytes &key, const TLBytes &iv)
{
    TLBytes encdata = data;
    TLBytes localiv = iv;

    AES_KEY aeskey;
    AES_set_encrypt_key(reinterpret_cast<const uchar*>(key.constData()), key.length() * 8, &aeskey);
    AES_ige_encrypt(reinterpret_cast<const uchar*>(data.constData()), reinterpret_cast<uchar*>(encdata.data()), data.length(), &aeskey, reinterpret_cast<uchar*>(localiv.data()), AES_ENCRYPT);
    return encdata;
}

QByteArray Aes::decrypt(const TLBytes &data, const TLBytes &key, const TLBytes &iv)
{
    TLBytes decdata = data;
    TLBytes localiv = iv;

    AES_KEY aeskey;
    AES_set_decrypt_key(reinterpret_cast<const uchar*>(key.constData()), key.length() * 8, &aeskey);
    AES_ige_encrypt(reinterpret_cast<const uchar*>(data.constData()), reinterpret_cast<uchar*>(decdata.data()), data.length(), &aeskey, reinterpret_cast<uchar*>(localiv.data()), AES_DECRYPT);
    return decdata;
}
