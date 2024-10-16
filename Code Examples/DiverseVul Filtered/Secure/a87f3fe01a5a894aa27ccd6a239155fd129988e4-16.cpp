void ssl_hmac_free(SSL_HMAC *ctx)
{
    if (ctx != NULL) {
        EVP_MAC_CTX_free(ctx->ctx);
#ifndef OPENSSL_NO_DEPRECATED_3_0
        HMAC_CTX_free(ctx->old_ctx);
#endif
        OPENSSL_free(ctx);
    }
}