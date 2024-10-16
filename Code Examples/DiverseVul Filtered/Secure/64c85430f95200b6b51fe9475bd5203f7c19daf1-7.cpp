OPENSSL_LHASH *OPENSSL_LH_new(OPENSSL_LH_HASHFUNC h, OPENSSL_LH_COMPFUNC c)
{
    OPENSSL_LHASH *ret;

    if ((ret = OPENSSL_zalloc(sizeof(*ret))) == NULL) {
        /*
         * Do not set the error code, because the ERR code uses LHASH
         * and we want to avoid possible endless error loop.
         * ERR_raise(ERR_LIB_CRYPTO, ERR_R_MALLOC_FAILURE);
         */
        return NULL;
    }
    if ((ret->b = OPENSSL_zalloc(sizeof(*ret->b) * MIN_NODES)) == NULL)
        goto err;
    ret->comp = ((c == NULL) ? (OPENSSL_LH_COMPFUNC)strcmp : c);
    ret->hash = ((h == NULL) ? (OPENSSL_LH_HASHFUNC)OPENSSL_LH_strhash : h);
    ret->num_nodes = MIN_NODES / 2;
    ret->num_alloc_nodes = MIN_NODES;
    ret->pmax = MIN_NODES / 2;
    ret->up_load = UP_LOAD;
    ret->down_load = DOWN_LOAD;
    return ret;

err:
    OPENSSL_free(ret->b);
    OPENSSL_free(ret);
    return NULL;
}