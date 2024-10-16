static int dsa_do_verify(const unsigned char *dgst, int dgst_len,
                         DSA_SIG *sig, DSA *dsa)
{
    BN_CTX *ctx;
    BIGNUM *u1, *u2, *t1;
    BN_MONT_CTX *mont = NULL;
    const BIGNUM *r, *s;
    int ret = -1, i;
    if (!dsa->p || !dsa->q || !dsa->g) {
        DSAerr(DSA_F_DSA_DO_VERIFY, DSA_R_MISSING_PARAMETERS);
        return -1;
    }

    i = BN_num_bits(dsa->q);
    /* fips 186-3 allows only different sizes for q */
    if (i != 160 && i != 224 && i != 256) {
        DSAerr(DSA_F_DSA_DO_VERIFY, DSA_R_BAD_Q_VALUE);
        return -1;
    }

    if (BN_num_bits(dsa->p) > OPENSSL_DSA_MAX_MODULUS_BITS) {
        DSAerr(DSA_F_DSA_DO_VERIFY, DSA_R_MODULUS_TOO_LARGE);
        return -1;
    }
    u1 = BN_new();
    u2 = BN_new();
    t1 = BN_new();
    ctx = BN_CTX_new();
    if (u1 == NULL || u2 == NULL || t1 == NULL || ctx == NULL)
        goto err;

    DSA_SIG_get0(sig, &r, &s);

    if (BN_is_zero(r) || BN_is_negative(r) ||
        BN_ucmp(r, dsa->q) >= 0) {
        ret = 0;
        goto err;
    }
    if (BN_is_zero(s) || BN_is_negative(s) ||
        BN_ucmp(s, dsa->q) >= 0) {
        ret = 0;
        goto err;
    }

    /*
     * Calculate W = inv(S) mod Q save W in u2
     */
    if ((BN_mod_inverse(u2, s, dsa->q, ctx)) == NULL)
        goto err;

    /* save M in u1 */
    if (dgst_len > (i >> 3))
        /*
         * if the digest length is greater than the size of q use the
         * BN_num_bits(dsa->q) leftmost bits of the digest, see fips 186-3,
         * 4.2
         */
        dgst_len = (i >> 3);
    if (BN_bin2bn(dgst, dgst_len, u1) == NULL)
        goto err;

    /* u1 = M * w mod q */
    if (!BN_mod_mul(u1, u1, u2, dsa->q, ctx))
        goto err;

    /* u2 = r * w mod q */
    if (!BN_mod_mul(u2, r, u2, dsa->q, ctx))
        goto err;

    if (dsa->flags & DSA_FLAG_CACHE_MONT_P) {
        mont = BN_MONT_CTX_set_locked(&dsa->method_mont_p,
                                      dsa->lock, dsa->p, ctx);
        if (!mont)
            goto err;
    }

    if (dsa->meth->dsa_mod_exp != NULL) {
        if (!dsa->meth->dsa_mod_exp(dsa, t1, dsa->g, u1, dsa->pub_key, u2,
                                    dsa->p, ctx, mont))
            goto err;
    } else {
        if (!BN_mod_exp2_mont(t1, dsa->g, u1, dsa->pub_key, u2, dsa->p, ctx,
                              mont))
            goto err;
    }

    /* let u1 = u1 mod q */
    if (!BN_mod(u1, t1, dsa->q, ctx))
        goto err;

    /*
     * V is now in u1.  If the signature is correct, it will be equal to R.
     */
    ret = (BN_ucmp(u1, r) == 0);

 err:
    if (ret < 0)
        DSAerr(DSA_F_DSA_DO_VERIFY, ERR_R_BN_LIB);
    BN_CTX_free(ctx);
    BN_free(u1);
    BN_free(u2);
    BN_free(t1);
    return ret;
}