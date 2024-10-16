static int SWTPM_HMAC(unsigned char *md, unsigned int *md_len,
                      const void *key, int key_len,
                      const unsigned char *in, uint32_t in_length,
                      const unsigned char *ivec, uint32_t ivec_length)
{
    OSSL_PARAM params[2];
    EVP_MAC_CTX *ctx;
    EVP_MAC *hmac;
    size_t outl;
    int ret = 0;

    hmac = EVP_MAC_fetch(NULL, OSSL_MAC_NAME_HMAC, NULL);
    if (!hmac)
        return 0;

    ctx = EVP_MAC_CTX_new(hmac);
    if (!ctx)
        goto err;

    params[0] = OSSL_PARAM_construct_utf8_string(OSSL_ALG_PARAM_DIGEST,
                                                 "sha256", 0);
    params[1] = OSSL_PARAM_construct_end();

    if (!EVP_MAC_init(ctx, key, key_len, params) ||
        !EVP_MAC_update(ctx, in, in_length))
        goto err;

    if (ivec &&
        !EVP_MAC_update(ctx, ivec, ivec_length))
        goto err;

    if (!EVP_MAC_final(ctx, md, &outl, *md_len))
        goto err;
    *md_len = outl;

    ret = 1;

err:
    EVP_MAC_CTX_free(ctx);
    EVP_MAC_free(hmac);

    return ret;
}