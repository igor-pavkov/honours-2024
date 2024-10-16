int main(int argc, char *argv[])
{
    BN_CTX *ctx;
    BIO *out;
    char *outfile = NULL;

    results = 0;

    RAND_seed(rnd_seed, sizeof rnd_seed); /* or BN_generate_prime may fail */

    argc--;
    argv++;
    while (argc >= 1) {
        if (strcmp(*argv, "-results") == 0)
            results = 1;
        else if (strcmp(*argv, "-out") == 0) {
            if (--argc < 1)
                break;
            outfile = *(++argv);
        }
        argc--;
        argv++;
    }

    ctx = BN_CTX_new();
    if (ctx == NULL)
        EXIT(1);

    out = BIO_new(BIO_s_file());
    if (out == NULL)
        EXIT(1);
    if (outfile == NULL) {
        BIO_set_fp(out, stdout, BIO_NOCLOSE);
    } else {
        if (!BIO_write_filename(out, outfile)) {
            perror(outfile);
            EXIT(1);
        }
    }

    if (!results)
        BIO_puts(out, "obase=16\nibase=16\n");

    message(out, "BN_add");
    if (!test_add(out))
        goto err;
    (void)BIO_flush(out);

    message(out, "BN_sub");
    if (!test_sub(out))
        goto err;
    (void)BIO_flush(out);

    message(out, "BN_lshift1");
    if (!test_lshift1(out))
        goto err;
    (void)BIO_flush(out);

    message(out, "BN_lshift (fixed)");
    if (!test_lshift(out, ctx, BN_bin2bn(lst, sizeof(lst) - 1, NULL)))
        goto err;
    (void)BIO_flush(out);

    message(out, "BN_lshift");
    if (!test_lshift(out, ctx, NULL))
        goto err;
    (void)BIO_flush(out);

    message(out, "BN_rshift1");
    if (!test_rshift1(out))
        goto err;
    (void)BIO_flush(out);

    message(out, "BN_rshift");
    if (!test_rshift(out, ctx))
        goto err;
    (void)BIO_flush(out);

    message(out, "BN_sqr");
    if (!test_sqr(out, ctx))
        goto err;
    (void)BIO_flush(out);

    message(out, "BN_mul");
    if (!test_mul(out))
        goto err;
    (void)BIO_flush(out);

    message(out, "BN_div");
    if (!test_div(out, ctx))
        goto err;
    (void)BIO_flush(out);

    message(out, "BN_div_word");
    if (!test_div_word(out))
        goto err;
    (void)BIO_flush(out);

    message(out, "BN_div_recp");
    if (!test_div_recp(out, ctx))
        goto err;
    (void)BIO_flush(out);

    message(out, "BN_mod");
    if (!test_mod(out, ctx))
        goto err;
    (void)BIO_flush(out);

    message(out, "BN_mod_mul");
    if (!test_mod_mul(out, ctx))
        goto err;
    (void)BIO_flush(out);

    message(out, "BN_mont");
    if (!test_mont(out, ctx))
        goto err;
    (void)BIO_flush(out);

    message(out, "BN_mod_exp");
    if (!test_mod_exp(out, ctx))
        goto err;
    (void)BIO_flush(out);

    message(out, "BN_mod_exp_mont_consttime");
    if (!test_mod_exp_mont_consttime(out, ctx))
        goto err;
    if (!test_mod_exp_mont5(out, ctx))
        goto err;
    (void)BIO_flush(out);

    message(out, "BN_exp");
    if (!test_exp(out, ctx))
        goto err;
    (void)BIO_flush(out);

    message(out, "BN_kronecker");
    if (!test_kron(out, ctx))
        goto err;
    (void)BIO_flush(out);

    message(out, "BN_mod_sqrt");
    if (!test_sqrt(out, ctx))
        goto err;
    (void)BIO_flush(out);
#ifndef OPENSSL_NO_EC2M
    message(out, "BN_GF2m_add");
    if (!test_gf2m_add(out))
        goto err;
    (void)BIO_flush(out);

    message(out, "BN_GF2m_mod");
    if (!test_gf2m_mod(out))
        goto err;
    (void)BIO_flush(out);

    message(out, "BN_GF2m_mod_mul");
    if (!test_gf2m_mod_mul(out, ctx))
        goto err;
    (void)BIO_flush(out);

    message(out, "BN_GF2m_mod_sqr");
    if (!test_gf2m_mod_sqr(out, ctx))
        goto err;
    (void)BIO_flush(out);

    message(out, "BN_GF2m_mod_inv");
    if (!test_gf2m_mod_inv(out, ctx))
        goto err;
    (void)BIO_flush(out);

    message(out, "BN_GF2m_mod_div");
    if (!test_gf2m_mod_div(out, ctx))
        goto err;
    (void)BIO_flush(out);

    message(out, "BN_GF2m_mod_exp");
    if (!test_gf2m_mod_exp(out, ctx))
        goto err;
    (void)BIO_flush(out);

    message(out, "BN_GF2m_mod_sqrt");
    if (!test_gf2m_mod_sqrt(out, ctx))
        goto err;
    (void)BIO_flush(out);

    message(out, "BN_GF2m_mod_solve_quad");
    if (!test_gf2m_mod_solve_quad(out, ctx))
        goto err;
    (void)BIO_flush(out);
#endif
    BN_CTX_free(ctx);
    BIO_free(out);

    EXIT(0);
 err:
    BIO_puts(out, "1\n");       /* make sure the Perl script fed by bc
                                 * notices the failure, see test_bn in
                                 * test/Makefile.ssl */
    (void)BIO_flush(out);
    ERR_load_crypto_strings();
    ERR_print_errors_fp(stderr);
    EXIT(1);
    return (1);
}