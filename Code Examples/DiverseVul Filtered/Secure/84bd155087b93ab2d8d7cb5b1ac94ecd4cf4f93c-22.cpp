static float gammapow(float x, float y)
{
#ifdef HAVE_FLDLN2
    register double logx;
    register long double v, e;
#else
    register float tmp, t, t2, r;
    int i;
#endif

    if(x == 0.0)
        return y == 0.0 ? 1.0 : 0.0;

#ifdef HAVE_FLDLN2
    /* FIXME: this can be optimised by directly calling fyl2x for x and y */
    asm volatile("fldln2; fxch; fyl2x"
                 : "=t" (logx) : "0" (x) : "st(1)");

    asm volatile("fldl2e\n\t"
                 "fmul %%st(1)\n\t"
                 "fst %%st(1)\n\t"
                 "frndint\n\t"
                 "fxch\n\t"
                 "fsub %%st(1)\n\t"
                 "f2xm1\n\t"
                 : "=t" (v), "=u" (e) : "0" (y * logx));
    v += 1.0;
    asm volatile("fscale"
                 : "=t" (v) : "0" (v), "u" (e));
    return v;
#else
    /* Compute ln(x) for x ∈ ]0,1]
     *   ln(x) = 2 * (t + t^3/3 + t^5/5 + ...) with t = (x-1)/(x+1)
     * The convergence is a bit slow, especially when x is near 0. */
    t = (x - 1.0) / (x + 1.0);
    t2 = t * t;
    tmp = r = t;
    for(i = 3; i < 20; i += 2)
    {
        r *= t2;
        tmp += r / i;
    }

    /* Compute -y*ln(x) */
    tmp = - y * 2.0 * tmp;

    /* Compute x^-y as e^t where t = -y*ln(x):
     *   e^t = 1 + t/1! + t^2/2! + t^3/3! + t^4/4! + t^5/5! ...
     * The convergence is quite faster here, thanks to the factorial. */
    r = t = tmp;
    tmp = 1.0 + t;
    for(i = 2; i < 16; i++)
    {
        r = r * t / i;
        tmp += r;
    }

    /* Return x^y as 1/(x^-y) */
    return 1.0 / tmp;
#endif
}