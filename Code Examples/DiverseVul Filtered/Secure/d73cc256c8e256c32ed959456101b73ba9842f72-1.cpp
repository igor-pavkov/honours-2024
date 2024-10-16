int test_gf2m_mod_div(BIO *bp, BN_CTX *ctx)
{
    BIGNUM *a, *b[2], *c, *d, *e, *f;
    int i, j, ret = 0;
    int p0[] = { 163, 7, 6, 3, 0, -1 };
    int p1[] = { 193, 15, 0, -1 };

    a = BN_new();
    b[0] = BN_new();
    b[1] = BN_new();
    c = BN_new();
    d = BN_new();
    e = BN_new();
    f = BN_new();

    BN_GF2m_arr2poly(p0, b[0]);
    BN_GF2m_arr2poly(p1, b[1]);

    for (i = 0; i < num0; i++) {
        BN_bntest_rand(a, 512, 0, 0);
        BN_bntest_rand(c, 512, 0, 0);
        for (j = 0; j < 2; j++) {
            BN_GF2m_mod_div(d, a, c, b[j], ctx);
            BN_GF2m_mod_mul(e, d, c, b[j], ctx);
            BN_GF2m_mod_div(f, a, e, b[j], ctx);
# if 0                          /* make test uses ouput in bc but bc can't
                                 * handle GF(2^m) arithmetic */
            if (bp != NULL) {
                if (!results) {
                    BN_print(bp, a);
                    BIO_puts(bp, " = ");
                    BN_print(bp, c);
                    BIO_puts(bp, " * ");
                    BN_print(bp, d);
                    BIO_puts(bp, " % ");
                    BN_print(bp, b[j]);
                    BIO_puts(bp, "\n");
                }
            }
# endif
            /* Test that ((a/c)*c)/a = 1. */
            if (!BN_is_one(f)) {
                fprintf(stderr, "GF(2^m) modular division test failed!\n");
                goto err;
            }
        }
    }
    ret = 1;
 err:
    BN_free(a);
    BN_free(b[0]);
    BN_free(b[1]);
    BN_free(c);
    BN_free(d);
    BN_free(e);
    BN_free(f);
    return ret;
}