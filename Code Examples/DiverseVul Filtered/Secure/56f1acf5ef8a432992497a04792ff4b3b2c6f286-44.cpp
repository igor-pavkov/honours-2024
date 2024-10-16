int SSL_CTX_set_ex_data(SSL_CTX *s, int idx, void *arg)
{
    return (CRYPTO_set_ex_data(&s->ex_data, idx, arg));
}