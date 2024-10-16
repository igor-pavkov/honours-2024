void BN_free(BIGNUM *a)
{
    if (a == NULL)
        return;
    bn_check_top(a);
    if (!BN_get_flags(a, BN_FLG_STATIC_DATA))
        bn_free_d(a);
    if (a->flags & BN_FLG_MALLOCED)
        OPENSSL_free(a);
    else {
#if OPENSSL_API_COMPAT < 0x00908000L
        a->flags |= BN_FLG_FREE;
#endif
        a->d = NULL;
    }
}