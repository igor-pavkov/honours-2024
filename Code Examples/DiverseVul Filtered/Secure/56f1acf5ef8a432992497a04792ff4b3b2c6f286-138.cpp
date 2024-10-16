void *SSL_CTX_get_ex_data(const SSL_CTX *s, int idx)
{
    return (CRYPTO_get_ex_data(&s->ex_data, idx));
}