long SSL_get_default_timeout(const SSL *s)
{
    return (s->method->get_timeout());
}