void X509_STORE_CTX_free(X509_STORE_CTX *ctx)
{
    if (!ctx)
        return;
    X509_STORE_CTX_cleanup(ctx);
    OPENSSL_free(ctx);
}