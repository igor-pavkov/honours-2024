STACK_OF(X509) *X509_STORE_CTX_get_chain(X509_STORE_CTX *ctx)
{
    return ctx->chain;
}