int test_mod_mul(BIO *bp, BN_CTX *ctx)
{
    BIGNUM *a, *b, *c, *d, *e;
    int i, j;

    a = BN_new();
    b = BN_new();
    c = BN_new();
    d = BN_new();
    e = BN_new();

    BN_one(a);
    BN_one(b);
    BN_zero(c);
    if (BN_mod_mul(e, a, b, c, ctx)) {
        fprintf(stderr, "BN_mod_mul with zero modulus succeeded!\n");
        return 0;
    }

    for (j = 0; j < 3; j++) {
        BN_bntest_rand(c, 1024, 0, 0);
        for (i = 0; i < num0; i++) {
            BN_bntest_rand(a, 475 + i * 10, 0, 0);
            BN_bntest_rand(b, 425 + i * 11, 0, 0);
            a->neg = rand_neg();
            b->neg = rand_neg();
            if (!BN_mod_mul(e, a, b, c, ctx)) {
                unsigned long l;

                while ((l = ERR_get_error()))
                    fprintf(stderr, "ERROR:%s\n", ERR_error_string(l, NULL));
                EXIT(1);
            }
            if (bp != NULL) {
                if (!results) {
                    BN_print(bp, a);
                    BIO_puts(bp, " * ");
                    BN_print(bp, b);
                    BIO_puts(bp, " % ");
                    BN_print(bp, c);
                    if ((a->neg ^ b->neg) && !BN_is_zero(e)) {
                        /*
                         * If (a*b) % c is negative, c must be added in order
                         * to obtain the normalized remainder (new with
                         * OpenSSL 0.9.7, previous versions of BN_mod_mul
                         * could generate negative results)
                         */
                        BIO_puts(bp, " + ");
                        BN_print(bp, c);
                    }
                    BIO_puts(bp, " - ");
                }
                BN_print(bp, e);
                BIO_puts(bp, "\n");
            }
            BN_mul(d, a, b, ctx);
            BN_sub(d, d, e);
            BN_div(a, b, d, c, ctx);
            if (!BN_is_zero(b)) {
                fprintf(stderr, "Modulo multiply test failed!\n");
                ERR_print_errors_fp(stderr);
                return 0;
            }
        }
    }
    BN_free(a);
    BN_free(b);
    BN_free(c);
    BN_free(d);
    BN_free(e);
    return (1);
}