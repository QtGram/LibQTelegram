#ifndef MATH_H
#define MATH_H

#include <QtGlobal>
#include <QByteArray>
#include <openssl/bn.h>
#include <openssl/rand.h>
#include "../types/basic.h"

class Math
{
    private:
        Math();

    public:
        static TLLong gcdOdd(TLLong a, TLLong b);
        static TLInt primeNumber(TLLong number);
        static TLBytes modExp(const TLBytes &n, const TLBytes &m, const TLBytes &e);
        static int padRequired(int len, int by);
        static void randomize(TLBytes& ba, int from = 0, int len = -1);

    public:
        template<typename T> static void randomize(T* t);
};

template<typename T> void Math::randomize(T* t) { RAND_bytes(reinterpret_cast<uchar*>(t), sizeof(T)); }

#endif // MATH_H
