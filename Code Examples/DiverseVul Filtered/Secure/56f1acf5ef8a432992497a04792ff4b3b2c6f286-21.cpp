int SSL_connect(SSL *s)
{
    if (s->handshake_func == 0)
        /* Not properly initialized yet */
        SSL_set_connect_state(s);

    return (s->method->ssl_connect(s));
}