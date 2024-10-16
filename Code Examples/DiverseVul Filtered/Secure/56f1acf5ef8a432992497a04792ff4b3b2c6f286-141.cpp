BIO *SSL_get_wbio(const SSL *s)
{
    return (s->wbio);
}