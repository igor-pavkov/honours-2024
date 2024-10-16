int test_gf2m_add(BIO *bp)
{
    BIGNUM a, b, c;
    int i, ret = 0;

    BN_init(&a);
    BN_init(&b);
    BN_init(&c);

    for (i = 0; i < num0; i++) {
        BN_rand(&a, 512, 0, 0);
        BN_copy(&b, BN_value_one());
        a.neg = rand_neg();
        b.neg = rand_neg();
        BN_GF2m_add(&c, &a, &b);
# if 0                          /* make test uses ouput in bc but bc can't
                                 * handle GF(2^m) arithmetic */
        if (bp != NULL) {
            if (!results) {
                BN_print(bp, &a);
                BIO_puts(bp, " ^ ");
                BN_print(bp, &b);
                BIO_puts(bp, " = ");
            }
            BN_print(bp, &c);
            BIO_puts(bp, "\n");
        }
# endif
        /* Test that two added values have the correct parity. */
        if ((BN_is_odd(&a) && BN_is_odd(&c))
            || (!BN_is_odd(&a) && !BN_is_odd(&c))) {
            fprintf(stderr, "GF(2^m) addition test (a) failed!\n");
            goto err;
        }
        BN_GF2m_add(&c, &c, &c);
        /* Test that c + c = 0. */
        if (!BN_is_zero(&c)) {
            fprintf(stderr, "GF(2^m) addition test (b) failed!\n");
            goto err;
        }
    }
    ret = 1;
 err:
    BN_free(&a);
    BN_free(&b);
    BN_free(&c);
    return ret;
}