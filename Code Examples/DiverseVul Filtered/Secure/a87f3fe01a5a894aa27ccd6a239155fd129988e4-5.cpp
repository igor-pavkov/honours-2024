static int rsa_pss_check_min_key_size(SSL_CTX *ctx, const EVP_PKEY *pkey,
                                      const SIGALG_LOOKUP *lu)
{
    const EVP_MD *md;

    if (pkey == NULL)
        return 0;
    if (!tls1_lookup_md(ctx, lu, &md) || md == NULL)
        return 0;
    if (EVP_PKEY_size(pkey) < RSA_PSS_MINIMUM_KEY_SIZE(md))
        return 0;
    return 1;
}