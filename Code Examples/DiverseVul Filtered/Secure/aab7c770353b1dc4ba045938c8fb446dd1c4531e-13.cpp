void BN_with_flags(BIGNUM *dest, const BIGNUM *b, int flags)
{
    dest->d = b->d;
    dest->top = b->top;
    dest->dmax = b->dmax;
    dest->neg = b->neg;
    dest->flags = ((dest->flags & BN_FLG_MALLOCED)
                   | (b->flags & ~BN_FLG_MALLOCED)
                   | BN_FLG_STATIC_DATA | flags);
}