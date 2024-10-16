void X509_STORE_CTX_set_cert(X509_STORE_CTX *ctx, X509 *x)
{
    ctx->cert = x;
}