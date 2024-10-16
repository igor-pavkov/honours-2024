int test_sqrt(BIO *bp, BN_CTX *ctx)
{
    BN_GENCB cb;
    BIGNUM *a, *p, *r;
    int i, j;
    int ret = 0;

    a = BN_new();
    p = BN_new();
    r = BN_new();
    if (a == NULL || p == NULL || r == NULL)
        goto err;

    BN_GENCB_set(&cb, genprime_cb, NULL);

    for (i = 0; i < 16; i++) {
        if (i < 8) {
            unsigned primes[8] = { 2, 3, 5, 7, 11, 13, 17, 19 };

            if (!BN_set_word(p, primes[i]))
                goto err;
        } else {
            if (!BN_set_word(a, 32))
                goto err;
            if (!BN_set_word(r, 2 * i + 1))
                goto err;

            if (!BN_generate_prime_ex(p, 256, 0, a, r, &cb))
                goto err;
            putc('\n', stderr);
        }
        p->neg = rand_neg();

        for (j = 0; j < num2; j++) {
            /*
             * construct 'a' such that it is a square modulo p, but in
             * general not a proper square and not reduced modulo p
             */
            if (!BN_bntest_rand(r, 256, 0, 3))
                goto err;
            if (!BN_nnmod(r, r, p, ctx))
                goto err;
            if (!BN_mod_sqr(r, r, p, ctx))
                goto err;
            if (!BN_bntest_rand(a, 256, 0, 3))
                goto err;
            if (!BN_nnmod(a, a, p, ctx))
                goto err;
            if (!BN_mod_sqr(a, a, p, ctx))
                goto err;
            if (!BN_mul(a, a, r, ctx))
                goto err;
            if (rand_neg())
                if (!BN_sub(a, a, p))
                    goto err;

            if (!BN_mod_sqrt(r, a, p, ctx))
                goto err;
            if (!BN_mod_sqr(r, r, p, ctx))
                goto err;

            if (!BN_nnmod(a, a, p, ctx))
                goto err;

            if (BN_cmp(a, r) != 0) {
                fprintf(stderr, "BN_mod_sqrt failed: a = ");
                BN_print_fp(stderr, a);
                fprintf(stderr, ", r = ");
                BN_print_fp(stderr, r);
                fprintf(stderr, ", p = ");
                BN_print_fp(stderr, p);
                fprintf(stderr, "\n");
                goto err;
            }

            putc('.', stderr);
            fflush(stderr);
        }

        putc('\n', stderr);
        fflush(stderr);
    }
    ret = 1;
 err:
    if (a != NULL)
        BN_free(a);
    if (p != NULL)
        BN_free(p);
    if (r != NULL)
        BN_free(r);
    return ret;
}