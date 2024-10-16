int SSL_peek(SSL *s, void *buf, int num)
{
    if (s->handshake_func == 0) {
        SSLerr(SSL_F_SSL_PEEK, SSL_R_UNINITIALIZED);
        return -1;
    }

    if (s->shutdown & SSL_RECEIVED_SHUTDOWN) {
        return (0);
    }
    return (s->method->ssl_peek(s, buf, num));
}