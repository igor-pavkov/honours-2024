static int dsa_sign_setup(DSA *dsa, BN_CTX *ctx_in,
                          BIGNUM **kinvp, BIGNUM **rp,
                          const unsigned char *dgst, int dlen)
{
    BN_CTX *ctx = NULL;
    BIGNUM *k, *kinv = NULL, *r = *rp;
    BIGNUM *l;
    int ret = 0;
    int q_bits, q_words;

    if (!dsa->p || !dsa->q || !dsa->g) {
        DSAerr(DSA_F_DSA_SIGN_SETUP, DSA_R_MISSING_PARAMETERS);
        return 0;
    }

    k = BN_new();
    l = BN_new();
    if (k == NULL || l == NULL)
        goto err;

    if (ctx_in == NULL) {
        if ((ctx = BN_CTX_new()) == NULL)
            goto err;
    } else
        ctx = ctx_in;

    /* Preallocate space */
    q_bits = BN_num_bits(dsa->q);
    q_words = bn_get_top(dsa->q);
    if (!bn_wexpand(k, q_words + 2)
        || !bn_wexpand(l, q_words + 2))
        goto err;

    /* Get random k */
    do {
        if (dgst != NULL) {
            /*
             * We calculate k from SHA512(private_key + H(message) + random).
             * This protects the private key from a weak PRNG.
             */
            if (!BN_generate_dsa_nonce(k, dsa->q, dsa->priv_key, dgst,
                                       dlen, ctx))
                goto err;
        } else if (!BN_priv_rand_range(k, dsa->q))
            goto err;
    } while (BN_is_zero(k));

    BN_set_flags(k, BN_FLG_CONSTTIME);

    if (dsa->flags & DSA_FLAG_CACHE_MONT_P) {
        if (!BN_MONT_CTX_set_locked(&dsa->method_mont_p,
                                    dsa->lock, dsa->p, ctx))
            goto err;
    }

    /* Compute r = (g^k mod p) mod q */

    /*
     * We do not want timing information to leak the length of k, so we
     * compute G^k using an equivalent scalar of fixed bit-length.
     *
     * We unconditionally perform both of these additions to prevent a
     * small timing information leakage.  We then choose the sum that is
     * one bit longer than the modulus.
     *
     * There are some concerns about the efficacy of doing this.  More
     * specificly refer to the discussion starting with:
     *     https://github.com/openssl/openssl/pull/7486#discussion_r228323705
     * The fix is to rework BN so these gymnastics aren't required.
     */
    if (!BN_add(l, k, dsa->q)
        || !BN_add(k, l, dsa->q))
        goto err;

    BN_consttime_swap(BN_is_bit_set(l, q_bits), k, l, q_words + 2);

    if ((dsa)->meth->bn_mod_exp != NULL) {
            if (!dsa->meth->bn_mod_exp(dsa, r, dsa->g, k, dsa->p, ctx,
                                       dsa->method_mont_p))
                goto err;
    } else {
            if (!BN_mod_exp_mont(r, dsa->g, k, dsa->p, ctx, dsa->method_mont_p))
                goto err;
    }

    if (!BN_mod(r, r, dsa->q, ctx))
        goto err;

    /* Compute part of 's = inv(k) (m + xr) mod q' */
    if ((kinv = dsa_mod_inverse_fermat(k, dsa->q, ctx)) == NULL)
        goto err;

    BN_clear_free(*kinvp);
    *kinvp = kinv;
    kinv = NULL;
    ret = 1;
 err:
    if (!ret)
        DSAerr(DSA_F_DSA_SIGN_SETUP, ERR_R_BN_LIB);
    if (ctx != ctx_in)
        BN_CTX_free(ctx);
    BN_clear_free(k);
    BN_clear_free(l);
    return ret;
}