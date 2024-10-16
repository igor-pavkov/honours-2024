void SSL_copy_session_id(SSL *t, const SSL *f)
{
    CERT *tmp;

    /* Do we need to to SSL locking? */
    SSL_set_session(t, SSL_get_session(f));

    /*
     * what if we are setup as SSLv2 but want to talk SSLv3 or vice-versa
     */
    if (t->method != f->method) {
        t->method->ssl_free(t); /* cleanup current */
        t->method = f->method;  /* change method */
        t->method->ssl_new(t);  /* setup new */
    }

    tmp = t->cert;
    if (f->cert != NULL) {
        CRYPTO_add(&f->cert->references, 1, CRYPTO_LOCK_SSL_CERT);
        t->cert = f->cert;
    } else
        t->cert = NULL;
    if (tmp != NULL)
        ssl_cert_free(tmp);
    SSL_set_session_id_context(t, f->sid_ctx, f->sid_ctx_length);
}