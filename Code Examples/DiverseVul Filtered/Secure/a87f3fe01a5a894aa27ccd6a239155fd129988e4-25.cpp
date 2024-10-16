HMAC_CTX *ssl_hmac_get0_HMAC_CTX(SSL_HMAC *ctx)
{
    return ctx->old_ctx;
}