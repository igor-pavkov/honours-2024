int test_mod(BIO *bp, BN_CTX *ctx)
{
    BIGNUM *a, *b, *c, *d, *e;
    int i;

    a = BN_new();
    b = BN_new();
    c = BN_new();
    d = BN_new();
    e = BN_new();

    BN_bntest_rand(a, 1024, 0, 0);
    for (i = 0; i < num0; i++) {
        BN_bntest_rand(b, 450 + i * 10, 0, 0);
        a->neg = rand_neg();
        b->neg = rand_neg();
        BN_mod(c, a, b, ctx);
        if (bp != NULL) {
            if (!results) {
                BN_print(bp, a);
                BIO_puts(bp, " % ");
                BN_print(bp, b);
                BIO_puts(bp, " - ");
            }
            BN_print(bp, c);
            BIO_puts(bp, "\n");
        }
        BN_div(d, e, a, b, ctx);
        BN_sub(e, e, c);
        if (!BN_is_zero(e)) {
            fprintf(stderr, "Modulo test failed!\n");
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