int SSL_set_ex_data(SSL *s, int idx, void *arg)
{
    return (CRYPTO_set_ex_data(&s->ex_data, idx, arg));
}