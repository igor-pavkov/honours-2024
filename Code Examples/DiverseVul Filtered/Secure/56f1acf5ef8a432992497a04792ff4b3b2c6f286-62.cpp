void SSL_set_verify_result(SSL *ssl, long arg)
{
    ssl->verify_result = arg;
}