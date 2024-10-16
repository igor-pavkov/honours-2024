int SSL_CTX_get_quiet_shutdown(const SSL_CTX *ctx)
{
    return (ctx->quiet_shutdown);
}