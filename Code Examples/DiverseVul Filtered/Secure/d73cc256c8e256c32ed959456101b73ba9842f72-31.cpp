int test_rshift1(BIO *bp)
{
    BIGNUM *a, *b, *c;
    int i;

    a = BN_new();
    b = BN_new();
    c = BN_new();

    BN_bntest_rand(a, 200, 0, 0);
    a->neg = rand_neg();
    for (i = 0; i < num0; i++) {
        BN_rshift1(b, a);
        if (bp != NULL) {
            if (!results) {
                BN_print(bp, a);
                BIO_puts(bp, " / 2");
                BIO_puts(bp, " - ");
            }
            BN_print(bp, b);
            BIO_puts(bp, "\n");
        }
        BN_sub(c, a, b);
        BN_sub(c, c, b);
        if (!BN_is_zero(c) && !BN_abs_is_word(c, 1)) {
            fprintf(stderr, "Right shift one test failed!\n");
            return 0;
        }
        BN_copy(a, b);
    }
    BN_free(a);
    BN_free(b);
    BN_free(c);
    return (1);
}