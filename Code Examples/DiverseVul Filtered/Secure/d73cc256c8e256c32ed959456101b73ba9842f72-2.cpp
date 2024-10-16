int test_gf2m_mod_solve_quad(BIO *bp, BN_CTX *ctx)
{
    BIGNUM *a, *b[2], *c, *d, *e;
    int i, j, s = 0, t, ret = 0;
    int p0[] = { 163, 7, 6, 3, 0, -1 };
    int p1[] = { 193, 15, 0, -1 };

    a = BN_new();
    b[0] = BN_new();
    b[1] = BN_new();
    c = BN_new();
    d = BN_new();
    e = BN_new();

    BN_GF2m_arr2poly(p0, b[0]);
    BN_GF2m_arr2poly(p1, b[1]);

    for (i = 0; i < num0; i++) {
        BN_bntest_rand(a, 512, 0, 0);
        for (j = 0; j < 2; j++) {
            t = BN_GF2m_mod_solve_quad(c, a, b[j], ctx);
            if (t) {
                s++;
                BN_GF2m_mod_sqr(d, c, b[j], ctx);
                BN_GF2m_add(d, c, d);
                BN_GF2m_mod(e, a, b[j]);
# if 0                          /* make test uses ouput in bc but bc can't
                                 * handle GF(2^m) arithmetic */
                if (bp != NULL) {
                    if (!results) {
                        BN_print(bp, c);
                        BIO_puts(bp, " is root of z^2 + z = ");
                        BN_print(bp, a);
                        BIO_puts(bp, " % ");
                        BN_print(bp, b[j]);
                        BIO_puts(bp, "\n");
                    }
                }
# endif
                BN_GF2m_add(e, e, d);
                /*
                 * Test that solution of quadratic c satisfies c^2 + c = a.
                 */
                if (!BN_is_zero(e)) {
                    fprintf(stderr,
                            "GF(2^m) modular solve quadratic test failed!\n");
                    goto err;
                }

            } else {
# if 0                          /* make test uses ouput in bc but bc can't
                                 * handle GF(2^m) arithmetic */
                if (bp != NULL) {
                    if (!results) {
                        BIO_puts(bp, "There are no roots of z^2 + z = ");
                        BN_print(bp, a);
                        BIO_puts(bp, " % ");
                        BN_print(bp, b[j]);
                        BIO_puts(bp, "\n");
                    }
                }
# endif
            }
        }
    }
    if (s == 0) {
        fprintf(stderr,
                "All %i tests of GF(2^m) modular solve quadratic resulted in no roots;\n",
                num0);
        fprintf(stderr,
                "this is very unlikely and probably indicates an error.\n");
        goto err;
    }
    ret = 1;
 err:
    BN_free(a);
    BN_free(b[0]);
    BN_free(b[1]);
    BN_free(c);
    BN_free(d);
    BN_free(e);
    return ret;
}