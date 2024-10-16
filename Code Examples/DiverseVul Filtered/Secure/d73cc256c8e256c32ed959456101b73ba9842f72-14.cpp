int test_mont(BIO *bp, BN_CTX *ctx)
{
    BIGNUM a, b, c, d, A, B;
    BIGNUM n;
    int i;
    BN_MONT_CTX *mont;

    BN_init(&a);
    BN_init(&b);
    BN_init(&c);
    BN_init(&d);
    BN_init(&A);
    BN_init(&B);
    BN_init(&n);

    mont = BN_MONT_CTX_new();
    if (mont == NULL)
        return 0;

    BN_zero(&n);
    if (BN_MONT_CTX_set(mont, &n, ctx)) {
        fprintf(stderr, "BN_MONT_CTX_set succeeded for zero modulus!\n");
        return 0;
    }

    BN_set_word(&n, 16);
    if (BN_MONT_CTX_set(mont, &n, ctx)) {
        fprintf(stderr, "BN_MONT_CTX_set succeeded for even modulus!\n");
        return 0;
    }

    BN_bntest_rand(&a, 100, 0, 0);
    BN_bntest_rand(&b, 100, 0, 0);
    for (i = 0; i < num2; i++) {
        int bits = (200 * (i + 1)) / num2;

        if (bits == 0)
            continue;
        BN_bntest_rand(&n, bits, 0, 1);
        BN_MONT_CTX_set(mont, &n, ctx);

        BN_nnmod(&a, &a, &n, ctx);
        BN_nnmod(&b, &b, &n, ctx);

        BN_to_montgomery(&A, &a, mont, ctx);
        BN_to_montgomery(&B, &b, mont, ctx);

        BN_mod_mul_montgomery(&c, &A, &B, mont, ctx);
        BN_from_montgomery(&A, &c, mont, ctx);
        if (bp != NULL) {
            if (!results) {
#ifdef undef
                fprintf(stderr, "%d * %d %% %d\n",
                        BN_num_bits(&a),
                        BN_num_bits(&b), BN_num_bits(mont->N));
#endif
                BN_print(bp, &a);
                BIO_puts(bp, " * ");
                BN_print(bp, &b);
                BIO_puts(bp, " % ");
                BN_print(bp, &(mont->N));
                BIO_puts(bp, " - ");
            }
            BN_print(bp, &A);
            BIO_puts(bp, "\n");
        }
        BN_mod_mul(&d, &a, &b, &n, ctx);
        BN_sub(&d, &d, &A);
        if (!BN_is_zero(&d)) {
            fprintf(stderr, "Montgomery multiplication test failed!\n");
            return 0;
        }
    }
    BN_MONT_CTX_free(mont);
    BN_free(&a);
    BN_free(&b);
    BN_free(&c);
    BN_free(&d);
    BN_free(&A);
    BN_free(&B);
    BN_free(&n);
    return (1);
}