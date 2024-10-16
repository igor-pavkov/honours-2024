size_t ssl_hmac_size(const SSL_HMAC *ctx)
{
    if (ctx->ctx != NULL)
        return EVP_MAC_size(ctx->ctx);
#ifndef OPENSSL_NO_DEPRECATED_3_0
    if (ctx->old_ctx != NULL)
        return HMAC_size(ctx->old_ctx);
#endif
    return 0;
}