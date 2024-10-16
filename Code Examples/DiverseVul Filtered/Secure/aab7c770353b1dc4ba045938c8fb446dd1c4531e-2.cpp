BIGNUM *BN_new(void)
{
    BIGNUM *ret;

    if ((ret = OPENSSL_zalloc(sizeof(*ret))) == NULL) {
        BNerr(BN_F_BN_NEW, ERR_R_MALLOC_FAILURE);
        return (NULL);
    }
    ret->flags = BN_FLG_MALLOCED;
    bn_check_top(ret);
    return (ret);
}