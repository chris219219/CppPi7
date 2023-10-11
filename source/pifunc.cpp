#include "../include/pifunc.h"

#include <float.h>

namespace pifunc
{
    char tohex(ufast8 a)
    {
        if (a > 9) return a + 55;
        return a + 48;
    }

    fast64 modpow(fast64 b, fast64 e, fast64 m)
    {
        fast64 res = 1;
        while (e)
        {
            if (e & 1)
                res = (res * b) % m;
            e >>= 1;
            b = b * b % m;
        }
        return res;
    }

    double fp(double a)
    {
        return a - ((fast64)(a + 10000) - 10000);
    }

    double sp(fast64 n, fast64 j)
    {
        double sum = 0;
        fast64 denomtop = j;
        double denom = j;

        while (n >= 0)
        {
            sum = fp(sum + ((double)modpow(16, n, denomtop) / denom));
            denomtop += 8;
            denom += 8.0;
            --n;
        }

        double num = 0.0625;
        num /= denom;

        while (num > DBL_EPSILON)
        {
            sum += num;
            num *= 0.0078125;
        }

        return sum;
    }

    ufast8 pidec(fast64 n)
    {
        double sum =
          4.0 * sp(n, 1)
        - 2.0 * sp(n, 4)
        -       sp(n, 5)
        -       sp(n, 6);
        return (ufast8)(16.0 * fp(sum));
    }

    char pihex(fast64 n)
    {
        return tohex(pidec(n));
    }
}