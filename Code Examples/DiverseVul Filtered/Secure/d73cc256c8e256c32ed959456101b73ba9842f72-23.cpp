int test_gf2m_mod_exp(BIO *bp, BN_CTX *ctx)
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
        BN_bntest_rand(d, 512, 0, 0);
        for (j = 0; j < 2; j++) {
            BN_GF2m_mod_exp(e, a, c, b[j], ctx);
            BN_GF2m_mod_exp(f, a, d, b[j], ctx);
            BN_GF2m_mod_mul(e, e, f, b[j], ctx);
            BN_add(f, c, d);
            BN_GF2m_mod_exp(f, a, f, b[j], ctx);
# if 0                          /* make test uses ouput in bc but bc can't
                                 * handle GF(2^m) arithmetic */
            if (bp != NULL) {
                if (!results) {
                    BN_print(bp, a);
                    BIO_puts(bp, " ^ (");
                    BN_print(bp, c);
                    BIO_puts(bp, " + ");
                    BN_print(bp, d);
                    BIO_puts(bp, ") = ");
                    BN_print(bp, e);
                    BIO_puts(bp, "; - ");
                    BN_print(bp, f);
                    BIO_puts(bp, " % ");
                    BN_print(bp, b[j]);
                    BIO_puts(bp, "\n");
                }
            }
# endif
            BN_GF2m_add(f, e, f);
            /* Test that a^(c+d)=a^c*a^d. */
            if (!BN_is_zero(f)) {
                fprintf(stderr,
                        "GF(2^m) modular exponentiation test failed!\n");
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