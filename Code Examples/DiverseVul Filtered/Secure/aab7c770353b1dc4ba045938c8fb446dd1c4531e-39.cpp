int BN_is_one(const BIGNUM *a)
{
    return BN_abs_is_word(a, 1) && !a->neg;
}