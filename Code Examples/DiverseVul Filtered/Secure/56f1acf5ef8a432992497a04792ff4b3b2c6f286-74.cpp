int SSL_CTX_set_default_verify_paths(SSL_CTX *ctx)
{
    return (X509_STORE_set_default_paths(ctx->cert_store));
}