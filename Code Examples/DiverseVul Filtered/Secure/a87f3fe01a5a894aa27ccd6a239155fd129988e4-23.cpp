int ssl_hmac_init(SSL_HMAC *ctx, void *key, size_t len, char *md)
{
    OSSL_PARAM params[3], *p = params;

    if (ctx->ctx != NULL) {
        *p++ = OSSL_PARAM_construct_utf8_string(OSSL_MAC_PARAM_DIGEST, md, 0);
        *p++ = OSSL_PARAM_construct_octet_string(OSSL_KDF_PARAM_KEY, key, len);
        *p = OSSL_PARAM_construct_end();
        if (EVP_MAC_CTX_set_params(ctx->ctx, params) && EVP_MAC_init(ctx->ctx))
            return 1;
    }
#ifndef OPENSSL_NO_DEPRECATED_3_0
    if (ctx->old_ctx != NULL)
        return HMAC_Init_ex(ctx->old_ctx, key, len,
                            EVP_get_digestbyname(md), NULL);
#endif
    return 0;
}