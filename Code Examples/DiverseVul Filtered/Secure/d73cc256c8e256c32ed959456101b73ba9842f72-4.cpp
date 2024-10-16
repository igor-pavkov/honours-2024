int test_mul(BIO *bp)
{
    BIGNUM a, b, c, d, e;
    int i;
    BN_CTX *ctx;

    ctx = BN_CTX_new();
    if (ctx == NULL)
        EXIT(1);

    BN_init(&a);
    BN_init(&b);
    BN_init(&c);
    BN_init(&d);
    BN_init(&e);

    for (i = 0; i < num0 + num1; i++) {
        if (i <= num1) {
            BN_bntest_rand(&a, 100, 0, 0);
            BN_bntest_rand(&b, 100, 0, 0);
        } else
            BN_bntest_rand(&b, i - num1, 0, 0);
        a.neg = rand_neg();
        b.neg = rand_neg();
        BN_mul(&c, &a, &b, ctx);
        if (bp != NULL) {
            if (!results) {
                BN_print(bp, &a);
                BIO_puts(bp, " * ");
                BN_print(bp, &b);
                BIO_puts(bp, " - ");
            }
            BN_print(bp, &c);
            BIO_puts(bp, "\n");
        }
        BN_div(&d, &e, &c, &a, ctx);
        BN_sub(&d, &d, &b);
        if (!BN_is_zero(&d) || !BN_is_zero(&e)) {
            fprintf(stderr, "Multiplication test failed!\n");
            return 0;
        }
    }
    BN_free(&a);
    BN_free(&b);
    BN_free(&c);
    BN_free(&d);
    BN_free(&e);
    BN_CTX_free(ctx);
    return (1);
}