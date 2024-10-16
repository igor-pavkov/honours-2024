int test_sub(BIO *bp)
{
    BIGNUM a, b, c;
    int i;

    BN_init(&a);
    BN_init(&b);
    BN_init(&c);

    for (i = 0; i < num0 + num1; i++) {
        if (i < num1) {
            BN_bntest_rand(&a, 512, 0, 0);
            BN_copy(&b, &a);
            if (BN_set_bit(&a, i) == 0)
                return (0);
            BN_add_word(&b, i);
        } else {
            BN_bntest_rand(&b, 400 + i - num1, 0, 0);
            a.neg = rand_neg();
            b.neg = rand_neg();
        }
        BN_sub(&c, &a, &b);
        if (bp != NULL) {
            if (!results) {
                BN_print(bp, &a);
                BIO_puts(bp, " - ");
                BN_print(bp, &b);
                BIO_puts(bp, " - ");
            }
            BN_print(bp, &c);
            BIO_puts(bp, "\n");
        }
        BN_add(&c, &c, &b);
        BN_sub(&c, &c, &a);
        if (!BN_is_zero(&c)) {
            fprintf(stderr, "Subtract test failed!\n");
            return 0;
        }
    }
    BN_free(&a);
    BN_free(&b);
    BN_free(&c);
    return (1);
}