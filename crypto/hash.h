#ifndef HASH_H
#define HASH_H

#include <QCryptographicHash>
#include "../types/basic.h"

#define Sha1_BitsCount  160
#define Sha1_BytesCount Sha1_BitsCount / 8

inline TLBytes md5_hash(const TLBytes& data) { return QCryptographicHash::hash(data, QCryptographicHash::Md5); }
inline TLBytes sha1_hash(const TLBytes& data) { return QCryptographicHash::hash(data, QCryptographicHash::Sha1); }
inline TLBytes sha256_hash(const TLBytes& data) { return QCryptographicHash::hash(data, QCryptographicHash::Sha256); }

#endif // HASH_H
