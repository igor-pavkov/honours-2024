int SSL_CTX_get_verify_mode(const SSL_CTX *ctx)
{
    return (ctx->verify_mode);
}