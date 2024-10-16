long SSL_get_verify_result(const SSL *ssl)
{
    return (ssl->verify_result);
}