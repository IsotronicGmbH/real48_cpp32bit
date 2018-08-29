/*                BPREAL                */
/* Borland Pascal Real-Type Conversions */
/*           by Richard Biffl           */


typedef unsigned real[3];

union doublearray {
    double d;
    unsigned a[4];
};

enum prconverr {
    prOK,             /* correct conversion (may be rounded) */
    prPosUnderflow,   /* pos. value, too small for Real */
    prNegUnderflow,   /* neg. value, too small for Real */
    prOverflow,       /* exponent too large for Real */
    prInf,            /* IEEE infinity, pos. or neg. */
    prNaN             /* IEEE NaN, not a number, e.g. sqrt(-1.0) */
} ;


double real_to_double (real r);

enum prconverr double_to_real (double d, real *r);


double real_to_double (real r)
/* takes Pascal real, return C double */
{
    union doublearray da;
    unsigned x;

    x = r[0] & 0x00FF;  /* Real biased exponent in x */
    /* when exponent is 0, value is 0.0 */
    if (x == 0)
        da.d = 0.0;
    else {
        da.a[3] = ((x + 894) << 4) |  /* adjust exponent bias */
                  (r[2] & 0x8000) |  /* sign bit */
                  ((r[2] & 0x7800) >> 11);  /* begin significand */
        da.a[2] = (r[2] << 5) |  /* continue shifting significand */
                  (r[1] >> 11);
        da.a[1] = (r[1] << 5) |
                  (r[0] >> 11);
        da.a[0] = (r[0] & 0xFF00) << 5; /* mask real's exponent */
    }
    return da.d;
}


enum prconverr double_to_real (double d, real *r)
/* converts C double to Pascal real, returns error code */
{
    union doublearray da;
    unsigned x;

    da.d = d;

    /* check for 0.0 */
    if ((da.a[0] == 0x0000) &&
        (da.a[1] == 0x0000) &&
        (da.a[2] == 0x0000) &&
        /* ignore sign bit */
        ((da.a[3] & 0x7FFF) == 0x0000)) {
        /* exponent and significand are both 0, so value is 0.0 */
        (*r)[2] = (*r)[1] = (*r)[0] = 0x0000;
        /* sign bit is ignored ( -0.0 -> 0.0 ) */
        return prOK;
    }

    /* test for maximum exponent value */
    if ((da.a[3] & 0x7FF0) == 0x7FF0) {
        /* value is either Inf or NaN */
        if ((da.a[0] == 0x0000) &&
            (da.a[1] == 0x0000) &&
            (da.a[2] == 0x0000) &&
            ((da.a[3] & 0x000F) == 0x0000)) {
            /* significand is 0, so value is Inf */
            /* value becomes signed maximum real, */
            /* and error code prInf is returned */
            (*r)[1] = (*r)[0] = 0xFFFF;
            (*r)[2] = 0x7FFF |
                      (da.a[3] & 0x8000); /* retain sign bit */
            return prInf;
        } else {
            /* significand is not 0, so value is NaN */
            /* value becomes 0.0, and prNaN code is returned */
            /* sign bit is ignored (no negative NaN) */
            (*r)[2] = (*r)[1] = (*r)[0] = 0x0000;
            /* sign bit is ignored ( -NaN -> +NaN ) */
            return prNaN;
        }
    }

    /* round significand if necessary */
    if ((da.a[0] & 0x1000) == 0x1000) {
        /* significand's 40th bit set, so round significand up */
        if ((da.a[0] & 0xE000) != 0xE000)
            /* room to increment 3 most significant bits */
            da.a[0] += 0x2000;
        else {
            /* carry bit to next element */
            da.a[0] = 0x0000;
            /* carry from 0th to 1st element */
            if (da.a[1] != 0xFFFF)
                da.a[1]++;
            else {
                da.a[1] = 0x0000;
                /* carry from 1st to 2nd element */
                if (da.a[2] != 0xFFFF)
                    da.a[2]++;
                else {
                    da.a[2] = 0x0000;
                    /* carry from 2nd to 3rd element */
                    /* significand may overflow into exponent */
                    /* exponent not full, so won't overflow */
                    da.a[3]++;
                }
            }
        }
    }

    /* get exponent for underflow/overflow tests */
    x = (da.a[3] & 0x7FF0) >> 4;

    /* test for underflow */
    if (x < 895) {
        /* value is below real range */
        (*r)[2] = (*r)[1] = (*r)[0] = 0x0000;
        if ((da.a[3] & 0x8000) == 0x8000)
            /* sign bit was set, so value was negative */
            return prNegUnderflow;
        else
            /* sign bit was not set */
            return prPosUnderflow;
    }

    /* test for overflow */
    if (x > 1149) {
        /* value is above real range */
        (*r)[1] = (*r)[0] = 0xFFFF;
        (*r)[2] = 0x7FFF | (da.a[3] & 0x8000); /* retain sign bit */
        return prOverflow;
    }

    /* value is within real range */
    (*r)[0] = (x - 894) |  /* re-bias exponent */
              ((da.a[0] & 0xE000) >> 5) |  /* begin significand */
              (da.a[1] << 11);
    (*r)[1] = (da.a[1] >> 5) |
              (da.a[2] << 11);
    (*r)[2] = (da.a[2] >> 5) |
              ((da.a[3] & 0x000F) << 11) |
              (da.a[3] & 0x8000);  /* copy sign bit */
    return prOK;

}
