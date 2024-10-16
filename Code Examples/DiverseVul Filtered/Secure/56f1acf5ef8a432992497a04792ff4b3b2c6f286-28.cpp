void *SSL_get_ex_data(const SSL *s, int idx)
{
    return (CRYPTO_get_ex_data(&s->ex_data, idx));
}