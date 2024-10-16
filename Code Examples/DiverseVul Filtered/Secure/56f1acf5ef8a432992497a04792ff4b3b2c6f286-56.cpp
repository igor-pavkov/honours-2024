int SSL_write(SSL *s, const void *buf, int num)
{
    if (s->handshake_func == 0) {
        SSLerr(SSL_F_SSL_WRITE, SSL_R_UNINITIALIZED);
        return -1;
    }

    if (s->shutdown & SSL_SENT_SHUTDOWN) {
        s->rwstate = SSL_NOTHING;
        SSLerr(SSL_F_SSL_WRITE, SSL_R_PROTOCOL_IS_SHUTDOWN);
        return (-1);
    }
    return (s->method->ssl_write(s, buf, num));
}