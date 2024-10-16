int test_lshift1(BIO *bp)
{
    BIGNUM *a, *b, *c;
    int i;

    a = BN_new();
    b = BN_new();
    c = BN_new();

    BN_bntest_rand(a, 200, 0, 0);
    a->neg = rand_neg();
    for (i = 0; i < num0; i++) {
        BN_lshift1(b, a);
        if (bp != NULL) {
            if (!results) {
                BN_print(bp, a);
                BIO_puts(bp, " * 2");
                BIO_puts(bp, " - ");
            }
            BN_print(bp, b);
            BIO_puts(bp, "\n");
        }
        BN_add(c, a, a);
        BN_sub(a, b, c);
        if (!BN_is_zero(a)) {
            fprintf(stderr, "Left shift one test failed!\n");
            return 0;
        }

        BN_copy(a, b);
    }
    BN_free(a);
    BN_free(b);
    BN_free(c);
    return (1);
}