int test_add(BIO *bp)
{
    BIGNUM a, b, c;
    int i;

    BN_init(&a);
    BN_init(&b);
    BN_init(&c);

    BN_bntest_rand(&a, 512, 0, 0);
    for (i = 0; i < num0; i++) {
        BN_bntest_rand(&b, 450 + i, 0, 0);
        a.neg = rand_neg();
        b.neg = rand_neg();
        BN_add(&c, &a, &b);
        if (bp != NULL) {
            if (!results) {
                BN_print(bp, &a);
                BIO_puts(bp, " + ");
                BN_print(bp, &b);
                BIO_puts(bp, " - ");
            }
            BN_print(bp, &c);
            BIO_puts(bp, "\n");
        }
        a.neg = !a.neg;
        b.neg = !b.neg;
        BN_add(&c, &c, &b);
        BN_add(&c, &c, &a);
        if (!BN_is_zero(&c)) {
            fprintf(stderr, "Add test failed!\n");
            return 0;
        }
    }
    BN_free(&a);
    BN_free(&b);
    BN_free(&c);
    return (1);
}