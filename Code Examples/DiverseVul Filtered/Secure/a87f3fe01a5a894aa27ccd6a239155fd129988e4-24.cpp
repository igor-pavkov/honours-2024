SSL_HMAC *ssl_hmac_new(const SSL_CTX *ctx)
{
    SSL_HMAC *ret = OPENSSL_zalloc(sizeof(*ret));
    EVP_MAC *mac = NULL;

    if (ret == NULL)
        return NULL;
#ifndef OPENSSL_NO_DEPRECATED_3_0
    if (ctx->ext.ticket_key_evp_cb == NULL
            && ctx->ext.ticket_key_cb != NULL) {
        ret->old_ctx = HMAC_CTX_new();
        if (ret->old_ctx == NULL)
            goto err;
        return ret;
    }
#endif
    mac = EVP_MAC_fetch(ctx->libctx, "HMAC", NULL);
    if (mac == NULL || (ret->ctx = EVP_MAC_CTX_new(mac)) == NULL)
        goto err;
    EVP_MAC_free(mac);
    return ret;
 err:
    EVP_MAC_CTX_free(ret->ctx);
    EVP_MAC_free(mac);
    OPENSSL_free(ret);
    return NULL;
}