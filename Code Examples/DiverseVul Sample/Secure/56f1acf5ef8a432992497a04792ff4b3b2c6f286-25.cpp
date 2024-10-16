int SSL_get_shutdown(const SSL *s)
{
    return (s->shutdown);
}