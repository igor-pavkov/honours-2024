void SSL_set_quiet_shutdown(SSL *s, int mode)
{
    s->quiet_shutdown = mode;
}