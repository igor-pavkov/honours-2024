void SSL_CTX_set_cert_store(SSL_CTX *ctx, X509_STORE *store)
{
    if (ctx->cert_store != NULL)
        X509_STORE_free(ctx->cert_store);
    ctx->cert_store = store;
}