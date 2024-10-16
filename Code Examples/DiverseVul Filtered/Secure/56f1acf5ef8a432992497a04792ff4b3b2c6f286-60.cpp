X509_STORE *SSL_CTX_get_cert_store(const SSL_CTX *ctx)
{
    return (ctx->cert_store);
}