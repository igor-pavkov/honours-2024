void BN_zero_ex(BIGNUM *a)
{
    a->top = 0;
    a->neg = 0;
}