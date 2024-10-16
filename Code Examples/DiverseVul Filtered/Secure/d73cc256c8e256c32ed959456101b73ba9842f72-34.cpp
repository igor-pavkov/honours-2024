int test_gf2m_mod_sqrt(BIO *bp, BN_CTX *ctx)
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
        for (j = 0; j < 2; j++) {
            BN_GF2m_mod(c, a, b[j]);
            BN_GF2m_mod_sqrt(d, a, b[j], ctx);
            BN_GF2m_mod_sqr(e, d, b[j], ctx);
# if 0                          /* make test uses ouput in bc but bc can't
                                 * handle GF(2^m) arithmetic */
            if (bp != NULL) {
                if (!results) {
                    BN_print(bp, d);
                    BIO_puts(bp, " ^ 2 - ");
                    BN_print(bp, a);
                    BIO_puts(bp, "\n");
                }
            }
# endif
            BN_GF2m_add(f, c, e);
            /* Test that d^2 = a, where d = sqrt(a). */
            if (!BN_is_zero(f)) {
                fprintf(stderr, "GF(2^m) modular square root test failed!\n");
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