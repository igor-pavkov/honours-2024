int SSL_do_handshake(SSL *s)
{
    int ret = 1;

    if (s->handshake_func == NULL) {
        SSLerr(SSL_F_SSL_DO_HANDSHAKE, SSL_R_CONNECTION_TYPE_NOT_SET);
        return (-1);
    }

    s->method->ssl_renegotiate_check(s);

    if (SSL_in_init(s) || SSL_in_before(s)) {
        ret = s->handshake_func(s);
    }
    return (ret);
}