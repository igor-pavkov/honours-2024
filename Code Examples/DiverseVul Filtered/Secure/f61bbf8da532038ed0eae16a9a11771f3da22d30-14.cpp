int BN_GF2m_mod_div(BIGNUM *r, const BIGNUM *y, const BIGNUM *x,
                    const BIGNUM *p, BN_CTX *ctx)
{
    BIGNUM *xinv = NULL;
    int ret = 0;

    bn_check_top(y);
    bn_check_top(x);
    bn_check_top(p);

    BN_CTX_start(ctx);
    xinv = BN_CTX_get(ctx);
    if (xinv == NULL)
        goto err;

    if (!BN_GF2m_mod_inv(xinv, x, p, ctx))
        goto err;
    if (!BN_GF2m_mod_mul(r, y, xinv, p, ctx))
        goto err;
    bn_check_top(r);
    ret = 1;

 err:
    BN_CTX_end(ctx);
    return ret;
}