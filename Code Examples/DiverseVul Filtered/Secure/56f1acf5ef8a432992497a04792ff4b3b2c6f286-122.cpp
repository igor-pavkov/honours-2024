int SSL_read(SSL *s, void *buf, int num)
{
    if (s->handshake_func == 0) {
        SSLerr(SSL_F_SSL_READ, SSL_R_UNINITIALIZED);
        return -1;
    }

    if (s->shutdown & SSL_RECEIVED_SHUTDOWN) {
        s->rwstate = SSL_NOTHING;
        return (0);
    }
    return (s->method->ssl_read(s, buf, num));
}