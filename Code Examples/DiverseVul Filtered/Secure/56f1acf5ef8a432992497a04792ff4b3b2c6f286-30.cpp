void SSL_set_shutdown(SSL *s, int mode)
{
    s->shutdown = mode;
}