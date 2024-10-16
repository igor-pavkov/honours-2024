int test_mod_exp_mont5(BIO *bp, BN_CTX *ctx)
{
    BIGNUM *a, *p, *m, *d, *e;
    BN_MONT_CTX *mont;

    a = BN_new();
    p = BN_new();
    m = BN_new();
    d = BN_new();
    e = BN_new();
    mont = BN_MONT_CTX_new();

    BN_bntest_rand(m, 1024, 0, 1); /* must be odd for montgomery */
    /* Zero exponent */
    BN_bntest_rand(a, 1024, 0, 0);
    BN_zero(p);
    if (!BN_mod_exp_mont_consttime(d, a, p, m, ctx, NULL))
        return 0;
    if (!BN_is_one(d)) {
        fprintf(stderr, "Modular exponentiation test failed!\n");
        return 0;
    }
    /* Zero input */
    BN_bntest_rand(p, 1024, 0, 0);
    BN_zero(a);
    if (!BN_mod_exp_mont_consttime(d, a, p, m, ctx, NULL))
        return 0;
    if (!BN_is_zero(d)) {
        fprintf(stderr, "Modular exponentiation test failed!\n");
        return 0;
    }
    /*
     * Craft an input whose Montgomery representation is 1, i.e., shorter
     * than the modulus m, in order to test the const time precomputation
     * scattering/gathering.
     */
    BN_one(a);
    BN_MONT_CTX_set(mont, m, ctx);
    if (!BN_from_montgomery(e, a, mont, ctx))
        return 0;
    if (!BN_mod_exp_mont_consttime(d, e, p, m, ctx, NULL))
        return 0;
    if (!BN_mod_exp_simple(a, e, p, m, ctx))
        return 0;
    if (BN_cmp(a, d) != 0) {
        fprintf(stderr, "Modular exponentiation test failed!\n");
        return 0;
    }
    /* Finally, some regular test vectors. */
    BN_bntest_rand(e, 1024, 0, 0);
    if (!BN_mod_exp_mont_consttime(d, e, p, m, ctx, NULL))
        return 0;
    if (!BN_mod_exp_simple(a, e, p, m, ctx))
        return 0;
    if (BN_cmp(a, d) != 0) {
        fprintf(stderr, "Modular exponentiation test failed!\n");
        return 0;
    }
    BN_MONT_CTX_free(mont);
    BN_free(a);
    BN_free(p);
    BN_free(m);
    BN_free(d);
    BN_free(e);
    return (1);
}