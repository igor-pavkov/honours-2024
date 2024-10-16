int SSL_get_read_ahead(const SSL *s)
{
    return (s->read_ahead);
}