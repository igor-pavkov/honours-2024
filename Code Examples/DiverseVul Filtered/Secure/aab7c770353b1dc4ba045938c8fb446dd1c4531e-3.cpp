int BN_is_negative(const BIGNUM *a)
{
    return (a->neg != 0);
}