static DSA_SIG *dsa_do_sign(const unsigned char *dgst, int dlen, DSA *dsa)
{
    BIGNUM *kinv = NULL;
    BIGNUM *m, *blind, *blindm, *tmp;
    BN_CTX *ctx = NULL;
    int reason = ERR_R_BN_LIB;
    DSA_SIG *ret = NULL;
    int rv = 0;

    if (dsa->p == NULL || dsa->q == NULL || dsa->g == NULL) {
        reason = DSA_R_MISSING_PARAMETERS;
        goto err;
    }

    ret = DSA_SIG_new();
    if (ret == NULL)
        goto err;
    ret->r = BN_new();
    ret->s = BN_new();
    if (ret->r == NULL || ret->s == NULL)
        goto err;

    ctx = BN_CTX_new();
    if (ctx == NULL)
        goto err;
    m = BN_CTX_get(ctx);
    blind = BN_CTX_get(ctx);
    blindm = BN_CTX_get(ctx);
    tmp = BN_CTX_get(ctx);
    if (tmp == NULL)
        goto err;

 redo:
    if (!dsa_sign_setup(dsa, ctx, &kinv, &ret->r, dgst, dlen))
        goto err;

    if (dlen > BN_num_bytes(dsa->q))
        /*
         * if the digest length is greater than the size of q use the
         * BN_num_bits(dsa->q) leftmost bits of the digest, see fips 186-3,
         * 4.2
         */
        dlen = BN_num_bytes(dsa->q);
    if (BN_bin2bn(dgst, dlen, m) == NULL)
        goto err;

    /*
     * The normal signature calculation is:
     *
     *   s := k^-1 * (m + r * priv_key) mod q
     *
     * We will blind this to protect against side channel attacks
     *
     *   s := blind^-1 * k^-1 * (blind * m + blind * r * priv_key) mod q
     */

    /* Generate a blinding value */
    do {
        if (!BN_priv_rand(blind, BN_num_bits(dsa->q) - 1,
                          BN_RAND_TOP_ANY, BN_RAND_BOTTOM_ANY))
            goto err;
    } while (BN_is_zero(blind));
    BN_set_flags(blind, BN_FLG_CONSTTIME);
    BN_set_flags(blindm, BN_FLG_CONSTTIME);
    BN_set_flags(tmp, BN_FLG_CONSTTIME);

    /* tmp := blind * priv_key * r mod q */
    if (!BN_mod_mul(tmp, blind, dsa->priv_key, dsa->q, ctx))
        goto err;
    if (!BN_mod_mul(tmp, tmp, ret->r, dsa->q, ctx))
        goto err;

    /* blindm := blind * m mod q */
    if (!BN_mod_mul(blindm, blind, m, dsa->q, ctx))
        goto err;

    /* s : = (blind * priv_key * r) + (blind * m) mod q */
    if (!BN_mod_add_quick(ret->s, tmp, blindm, dsa->q))
        goto err;

    /* s := s * k^-1 mod q */
    if (!BN_mod_mul(ret->s, ret->s, kinv, dsa->q, ctx))
        goto err;

    /* s:= s * blind^-1 mod q */
    if (BN_mod_inverse(blind, blind, dsa->q, ctx) == NULL)
        goto err;
    if (!BN_mod_mul(ret->s, ret->s, blind, dsa->q, ctx))
        goto err;

    /*
     * Redo if r or s is zero as required by FIPS 186-3: this is very
     * unlikely.
     */
    if (BN_is_zero(ret->r) || BN_is_zero(ret->s))
        goto redo;

    rv = 1;

 err:
    if (rv == 0) {
        DSAerr(DSA_F_DSA_DO_SIGN, reason);
        DSA_SIG_free(ret);
        ret = NULL;
    }
    BN_CTX_free(ctx);
    BN_clear_free(kinv);
    return ret;
}