int BN_is_odd(const BIGNUM *a)
{
    return (a->top > 0) && (a->d[0] & 1);
}