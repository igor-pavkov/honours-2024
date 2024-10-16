int SSL_CTX_load_verify_locations(SSL_CTX *ctx, const char *CAfile,
                                  const char *CApath)
{
    return (X509_STORE_load_locations(ctx->cert_store, CAfile, CApath));
}