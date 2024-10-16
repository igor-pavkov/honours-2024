void BN_clear(BIGNUM *a)
{
    bn_check_top(a);
    if (a->d != NULL)
        OPENSSL_cleanse(a->d, sizeof(*a->d) * a->dmax);
    a->top = 0;
    a->neg = 0;
}