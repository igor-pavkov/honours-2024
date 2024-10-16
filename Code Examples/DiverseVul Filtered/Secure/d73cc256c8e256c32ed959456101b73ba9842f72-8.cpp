int test_div(BIO *bp, BN_CTX *ctx)
{
    BIGNUM a, b, c, d, e;
    int i;

    BN_init(&a);
    BN_init(&b);
    BN_init(&c);
    BN_init(&d);
    BN_init(&e);

    BN_one(&a);
    BN_zero(&b);

    if (BN_div(&d, &c, &a, &b, ctx)) {
        fprintf(stderr, "Division by zero succeeded!\n");
        return 0;
    }

    for (i = 0; i < num0 + num1; i++) {
        if (i < num1) {
            BN_bntest_rand(&a, 400, 0, 0);
            BN_copy(&b, &a);
            BN_lshift(&a, &a, i);
            BN_add_word(&a, i);
        } else
            BN_bntest_rand(&b, 50 + 3 * (i - num1), 0, 0);
        a.neg = rand_neg();
        b.neg = rand_neg();
        BN_div(&d, &c, &a, &b, ctx);
        if (bp != NULL) {
            if (!results) {
                BN_print(bp, &a);
                BIO_puts(bp, " / ");
                BN_print(bp, &b);
                BIO_puts(bp, " - ");
            }
            BN_print(bp, &d);
            BIO_puts(bp, "\n");

            if (!results) {
                BN_print(bp, &a);
                BIO_puts(bp, " % ");
                BN_print(bp, &b);
                BIO_puts(bp, " - ");
            }
            BN_print(bp, &c);
            BIO_puts(bp, "\n");
        }
        BN_mul(&e, &d, &b, ctx);
        BN_add(&d, &e, &c);
        BN_sub(&d, &d, &a);
        if (!BN_is_zero(&d)) {
            fprintf(stderr, "Division test failed!\n");
            return 0;
        }
    }
    BN_free(&a);
    BN_free(&b);
    BN_free(&c);
    BN_free(&d);
    BN_free(&e);
    return (1);
}