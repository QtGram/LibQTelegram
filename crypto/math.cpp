#include "math.h"

Math::Math()
{

}

TLLong Math::gcdOdd(TLLong a, TLLong b)
{
    while(a != 0 && b != 0)
    {
        while(!(b & 1))
            b >>= 1;

        while(!(a & 1))
            a >>= 1;

        if(a > b)
            a -= b;
        else
            b -= a;
    }

    return b == 0 ? a : b;
}

// https://github.com/DrKLO/Telegram/blob/433f59c5b9ed17543d8e206c83f0bc7c7edb43a6/TMessagesProj/jni/jni.c#L86
// https://github.com/ex3ndr/telegram-mt/blob/91b1186e567b0484d6f371b8e5c61c425daf867e/src/main/java/org/telegram/mtproto/secure/pq/PQLopatin.java#L35
// https://github.com/vysheng/tg/blob/1dad2e89933085ea1e3d9fb1becb1907ce5de55f/mtproto-client.c#L296
TLInt Math::primeNumber(TLLong number)
{
    int it = 0;
    TLLong g = 0;

    for(int i = 0; i < 3 || it < 10000; i++)
    {
        const TLLong q = ((rand() & 15) + 17) % number;
        TLLong x = (TLLong) rand() % (number - 1) + 1;
        TLLong y = x;
        const TLInt lim = 1 << (i + 18);

        for(TLInt j = 1; j < lim; j++)
        {
            ++it;
            TLLong a = x;
            TLLong b = x;
            TLLong c = q;

            while(b)
            {
                if(b & 1)
                {
                    c += a;

                    if(c >= number)
                        c -= number;
                }

                a += a;

                if(a >= number)
                    a -= number;

                b >>= 1;
            }

            x = c;
            const TLLong z = x < y ? number + x - y : x - y;
            g = Math::gcdOdd(z, number);

            if (g != 1)
                return g;

            if(!(j & (j - 1)))
                y = x;
        }

        if(g > 1 && g < number)
            return g;
    }

    return 1;
}

TLBytes Math::modExp(const TLBytes &n, const TLBytes &m, const TLBytes &e)
{
    TLBytes res;
    BN_CTX* bnctx = BN_CTX_new();
    BIGNUM *bnres = BN_new(), *bnn = BN_new(), *bnm = BN_new(), *bne = BN_new();

    res.fill(0, 256);

    BN_bin2bn(reinterpret_cast<const uchar*>(n.constData()), n.length(), bnn);
    BN_bin2bn(reinterpret_cast<const uchar*>(m.constData()), m.length(), bnm);
    BN_bin2bn(reinterpret_cast<const uchar*>(e.constData()), e.length(), bne);

    BN_mod_exp(bnres, bnn, bne, bnm, bnctx);
    BN_bn2bin(bnres, reinterpret_cast<uchar*>(res.data()));

    BN_free(bne);
    BN_free(bnm);
    BN_free(bnn);
    BN_free(bnres);
    BN_CTX_free(bnctx);
    return res;
}

int Math::padRequired(int len, int by)
{
    int i = len;

    while(i % by)
        i++;

    return i - len;
}

void Math::randomize(TLBytes &ba, int from, int len)
{
   RAND_bytes(reinterpret_cast<uchar*>(ba.data() + from), (len == -1 ? ba.length() : len));
}
