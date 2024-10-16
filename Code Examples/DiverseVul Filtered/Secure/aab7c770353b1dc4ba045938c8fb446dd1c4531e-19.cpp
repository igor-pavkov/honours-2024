static void bn_free_d(BIGNUM *a)
{
    if (BN_get_flags(a, BN_FLG_SECURE))
        OPENSSL_secure_free(a->d);
    else
        OPENSSL_free(a->d);
}