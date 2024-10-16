static DSA_SIG *dsa_do_sign(const unsigned char *dgst, int dlen, DSA *dsa)
{
    BIGNUM *kinv = NULL, *r = NULL, *s = NULL;
    BIGNUM m;
    BIGNUM xr;
    BN_CTX *ctx = NULL;
    int reason = ERR_R_BN_LIB;
    DSA_SIG *ret = NULL;
    int noredo = 0;

    BN_init(&m);
    BN_init(&xr);

    if (!dsa->p || !dsa->q || !dsa->g) {
        reason = DSA_R_MISSING_PARAMETERS;
        goto err;
    }

    s = BN_new();
    if (s == NULL)
        goto err;
    ctx = BN_CTX_new();
    if (ctx == NULL)
        goto err;
 redo:
    if ((dsa->kinv == NULL) || (dsa->r == NULL)) {
        if (!DSA_sign_setup(dsa, ctx, &kinv, &r))
            goto err;
    } else {
        kinv = dsa->kinv;
        dsa->kinv = NULL;
        r = dsa->r;
        dsa->r = NULL;
        noredo = 1;
    }

    if (dlen > BN_num_bytes(dsa->q))
        /*
         * if the digest length is greater than the size of q use the
         * BN_num_bits(dsa->q) leftmost bits of the digest, see fips 186-3,
         * 4.2
         */
        dlen = BN_num_bytes(dsa->q);
    if (BN_bin2bn(dgst, dlen, &m) == NULL)
        goto err;

    /* Compute  s = inv(k) (m + xr) mod q */
    if (!BN_mod_mul(&xr, dsa->priv_key, r, dsa->q, ctx))
        goto err;               /* s = xr */
    if (!BN_add(s, &xr, &m))
        goto err;               /* s = m + xr */
    if (BN_cmp(s, dsa->q) > 0)
        if (!BN_sub(s, s, dsa->q))
            goto err;
    if (!BN_mod_mul(s, s, kinv, dsa->q, ctx))
        goto err;

    /*
     * Redo if r or s is zero as required by FIPS 186-3: this is very
     * unlikely.
     */
    if (BN_is_zero(r) || BN_is_zero(s)) {
        if (noredo) {
            reason = DSA_R_NEED_NEW_SETUP_VALUES;
            goto err;
        }
        goto redo;
    }
    ret = DSA_SIG_new();
    if (ret == NULL)
        goto err;
    ret->r = r;
    ret->s = s;

 err:
    if (ret == NULL) {
        DSAerr(DSA_F_DSA_DO_SIGN, reason);
        BN_free(r);
        BN_free(s);
    }
    if (ctx != NULL)
        BN_CTX_free(ctx);
    BN_clear_free(&m);
    BN_clear_free(&xr);
    if (kinv != NULL)           /* dsa->kinv is NULL now if we used it */
        BN_clear_free(kinv);
    return (ret);
}