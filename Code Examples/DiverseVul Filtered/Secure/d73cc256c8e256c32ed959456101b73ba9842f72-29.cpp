int test_lshift(BIO *bp, BN_CTX *ctx, BIGNUM *a_)
{
    BIGNUM *a, *b, *c, *d;
    int i;

    b = BN_new();
    c = BN_new();
    d = BN_new();
    BN_one(c);

    if (a_)
        a = a_;
    else {
        a = BN_new();
        BN_bntest_rand(a, 200, 0, 0);
        a->neg = rand_neg();
    }
    for (i = 0; i < num0; i++) {
        BN_lshift(b, a, i + 1);
        BN_add(c, c, c);
        if (bp != NULL) {
            if (!results) {
                BN_print(bp, a);
                BIO_puts(bp, " * ");
                BN_print(bp, c);
                BIO_puts(bp, " - ");
            }
            BN_print(bp, b);
            BIO_puts(bp, "\n");
        }
        BN_mul(d, a, c, ctx);
        BN_sub(d, d, b);
        if (!BN_is_zero(d)) {
            fprintf(stderr, "Left shift test failed!\n");
            fprintf(stderr, "a=");
            BN_print_fp(stderr, a);
            fprintf(stderr, "\nb=");
            BN_print_fp(stderr, b);
            fprintf(stderr, "\nc=");
            BN_print_fp(stderr, c);
            fprintf(stderr, "\nd=");
            BN_print_fp(stderr, d);
            fprintf(stderr, "\n");
            return 0;
        }
    }
    BN_free(a);
    BN_free(b);
    BN_free(c);
    BN_free(d);
    return (1);
}