BIO *SSL_get_rbio(const SSL *s)
{
    return (s->rbio);
}