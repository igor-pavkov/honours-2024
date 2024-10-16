BN_GENCB *BN_GENCB_new(void)
{
    BN_GENCB *ret;

    if ((ret = OPENSSL_malloc(sizeof(*ret))) == NULL) {
        BNerr(BN_F_BN_GENCB_NEW, ERR_R_MALLOC_FAILURE);
        return (NULL);
    }

    return ret;
}