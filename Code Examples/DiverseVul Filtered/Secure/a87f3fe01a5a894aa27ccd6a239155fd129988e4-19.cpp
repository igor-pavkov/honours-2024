int ssl_hmac_update(SSL_HMAC *ctx, const unsigned char *data, size_t len)
{
    if (ctx->ctx != NULL)
        return EVP_MAC_update(ctx->ctx, data, len);
#ifndef OPENSSL_NO_DEPRECATED_3_0
    if (ctx->old_ctx != NULL)
        return HMAC_Update(ctx->old_ctx, data, len);
#endif
    return 0;
}