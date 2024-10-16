int SSL_renegotiate(SSL *s)
{
    if (s->renegotiate == 0)
        s->renegotiate = 1;

    s->new_session = 1;

    return (s->method->ssl_renegotiate(s));
}