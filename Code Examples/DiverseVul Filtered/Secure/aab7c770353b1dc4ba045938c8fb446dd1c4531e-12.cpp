int BN_is_word(const BIGNUM *a, const BN_ULONG w)
{
    return BN_abs_is_word(a, w) && (!w || !a->neg);
}