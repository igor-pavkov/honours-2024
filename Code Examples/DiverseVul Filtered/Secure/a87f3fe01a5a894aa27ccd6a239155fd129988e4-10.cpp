EVP_MAC_CTX *ssl_hmac_get0_EVP_MAC_CTX(SSL_HMAC *ctx)
{
    return ctx->ctx;
}