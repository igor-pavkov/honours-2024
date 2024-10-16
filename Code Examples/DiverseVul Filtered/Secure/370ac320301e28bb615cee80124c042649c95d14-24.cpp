X509_STORE_CTX *X509_STORE_CTX_new(void)
{
    X509_STORE_CTX *ctx;
    ctx = (X509_STORE_CTX *)OPENSSL_malloc(sizeof(X509_STORE_CTX));
    if (!ctx) {
        X509err(X509_F_X509_STORE_CTX_NEW, ERR_R_MALLOC_FAILURE);
        return NULL;
    }
    memset(ctx, 0, sizeof(X509_STORE_CTX));
    return ctx;
}