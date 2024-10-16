int SSL_renegotiate_abbreviated(SSL *s)
{
    if (s->renegotiate == 0)
        s->renegotiate = 1;

    s->new_session = 0;

    return (s->method->ssl_renegotiate(s));
}