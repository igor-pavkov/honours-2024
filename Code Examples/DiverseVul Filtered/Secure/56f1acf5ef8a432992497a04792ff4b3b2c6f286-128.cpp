SSL_CTX *SSL_get_SSL_CTX(const SSL *ssl)
{
    return (ssl->ctx);
}