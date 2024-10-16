int SSL_get_quiet_shutdown(const SSL *s)
{
    return (s->quiet_shutdown);
}