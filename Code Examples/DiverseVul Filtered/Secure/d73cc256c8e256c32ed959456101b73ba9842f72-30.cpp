int test_div_recp(BIO *bp, BN_CTX *ctx)
{
    BIGNUM a, b, c, d, e;
    BN_RECP_CTX recp;
    int i;

    BN_RECP_CTX_init(&recp);
    BN_init(&a);
    BN_init(&b);
    BN_init(&c);
    BN_init(&d);
    BN_init(&e);

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
        BN_RECP_CTX_set(&recp, &b, ctx);
        BN_div_recp(&d, &c, &a, &recp, ctx);
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
            fprintf(stderr, "Reciprocal division test failed!\n");
            fprintf(stderr, "a=");
            BN_print_fp(stderr, &a);
            fprintf(stderr, "\nb=");
            BN_print_fp(stderr, &b);
            fprintf(stderr, "\n");
            return 0;
        }
    }
    BN_free(&a);
    BN_free(&b);
    BN_free(&c);
    BN_free(&d);
    BN_free(&e);
    BN_RECP_CTX_free(&recp);
    return (1);
}