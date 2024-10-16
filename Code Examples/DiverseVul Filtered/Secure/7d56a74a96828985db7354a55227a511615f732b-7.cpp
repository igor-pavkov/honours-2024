static int rc4_hmac_md5_dinit(void *ctx, const unsigned char *key,
                              size_t keylen, const unsigned char *iv,
                              size_t ivlen, const OSSL_PARAM params[])
{
    if (!ossl_cipher_generic_dinit(ctx, key, keylen, iv, ivlen, NULL))
        return 0;
    return rc4_hmac_md5_set_ctx_params(ctx, params);
}