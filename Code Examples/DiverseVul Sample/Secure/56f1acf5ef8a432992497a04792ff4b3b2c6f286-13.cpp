X509 *SSL_get_peer_certificate(const SSL *s)
{
    X509 *r;

    if ((s == NULL) || (s->session == NULL))
        r = NULL;
    else
        r = s->session->peer;

    if (r == NULL)
        return (r);

    CRYPTO_add(&r->references, 1, CRYPTO_LOCK_X509);

    return (r);
}