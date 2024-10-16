static BIGNUM *dsa_mod_inverse_fermat(const BIGNUM *k, const BIGNUM *q,
                                      BN_CTX *ctx)
{
    BIGNUM *res = NULL;
    BIGNUM *r, *e;

    if ((r = BN_new()) == NULL)
        return NULL;

    BN_CTX_start(ctx);
    if ((e = BN_CTX_get(ctx)) != NULL
            && BN_set_word(r, 2)
            && BN_sub(e, q, r)
            && BN_mod_exp_mont(r, k, e, q, ctx, NULL))
        res = r;
    else
        BN_free(r);
    BN_CTX_end(ctx);
    return res;
}