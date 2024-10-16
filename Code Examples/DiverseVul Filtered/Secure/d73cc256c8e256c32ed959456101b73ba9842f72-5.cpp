int test_rshift(BIO *bp, BN_CTX *ctx)
{
    BIGNUM *a, *b, *c, *d, *e;
    int i;

    a = BN_new();
    b = BN_new();
    c = BN_new();
    d = BN_new();
    e = BN_new();
    BN_one(c);

    BN_bntest_rand(a, 200, 0, 0);
    a->neg = rand_neg();
    for (i = 0; i < num0; i++) {
        BN_rshift(b, a, i + 1);
        BN_add(c, c, c);
        if (bp != NULL) {
            if (!results) {
                BN_print(bp, a);
                BIO_puts(bp, " / ");
                BN_print(bp, c);
                BIO_puts(bp, " - ");
            }
            BN_print(bp, b);
            BIO_puts(bp, "\n");
        }
        BN_div(d, e, a, c, ctx);
        BN_sub(d, d, b);
        if (!BN_is_zero(d)) {
            fprintf(stderr, "Right shift test failed!\n");
            return 0;
        }
    }
    BN_free(a);
    BN_free(b);
    BN_free(c);
    BN_free(d);
    BN_free(e);
    return (1);
}