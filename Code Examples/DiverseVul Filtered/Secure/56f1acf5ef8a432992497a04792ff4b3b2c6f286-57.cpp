SSL_CTX *SSL_set_SSL_CTX(SSL *ssl, SSL_CTX *ctx)
{
    CERT *ocert = ssl->cert;
    if (ssl->ctx == ctx)
        return ssl->ctx;
#ifndef OPENSSL_NO_TLSEXT
    if (ctx == NULL)
        ctx = ssl->initial_ctx;
#endif
    ssl->cert = ssl_cert_dup(ctx->cert);
    if (ocert != NULL) {
        int i;
        /* Copy negotiated digests from original */
        for (i = 0; i < SSL_PKEY_NUM; i++) {
            CERT_PKEY *cpk = ocert->pkeys + i;
            CERT_PKEY *rpk = ssl->cert->pkeys + i;
            rpk->digest = cpk->digest;
        }
        ssl_cert_free(ocert);
    }

    /*
     * Program invariant: |sid_ctx| has fixed size (SSL_MAX_SID_CTX_LENGTH),
     * so setter APIs must prevent invalid lengths from entering the system.
     */
    OPENSSL_assert(ssl->sid_ctx_length <= sizeof(ssl->sid_ctx));

    /*
     * If the session ID context matches that of the parent SSL_CTX,
     * inherit it from the new SSL_CTX as well. If however the context does
     * not match (i.e., it was set per-ssl with SSL_set_session_id_context),
     * leave it unchanged.
     */
    if ((ssl->ctx != NULL) &&
        (ssl->sid_ctx_length == ssl->ctx->sid_ctx_length) &&
        (memcmp(ssl->sid_ctx, ssl->ctx->sid_ctx, ssl->sid_ctx_length) == 0)) {
        ssl->sid_ctx_length = ctx->sid_ctx_length;
        memcpy(&ssl->sid_ctx, &ctx->sid_ctx, sizeof(ssl->sid_ctx));
    }

    CRYPTO_add(&ctx->references, 1, CRYPTO_LOCK_SSL_CTX);
    if (ssl->ctx != NULL)
        SSL_CTX_free(ssl->ctx); /* decrement reference count */
    ssl->ctx = ctx;

    return (ssl->ctx);
}