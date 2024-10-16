STACK_OF(X509) *SSL_get_peer_cert_chain(const SSL *s)
{
    STACK_OF(X509) *r;

    if ((s == NULL) || (s->session == NULL)
        || (s->session->sess_cert == NULL))
        r = NULL;
    else
        r = s->session->sess_cert->cert_chain;

    /*
     * If we are a client, cert_chain includes the peer's own certificate; if
     * we are a server, it does not.
     */

    return (r);
}