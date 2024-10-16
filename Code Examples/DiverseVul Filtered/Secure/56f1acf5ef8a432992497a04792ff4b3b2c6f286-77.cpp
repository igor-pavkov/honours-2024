void ssl_free_wbio_buffer(SSL *s)
{
    if (s->bbio == NULL)
        return;

    if (s->bbio == s->wbio) {
        /* remove buffering */
        s->wbio = BIO_pop(s->wbio);
#ifdef REF_CHECK                /* not the usual REF_CHECK, but this avoids
                                 * adding one more preprocessor symbol */
        assert(s->wbio != NULL);
#endif
    }
    BIO_free(s->bbio);
    s->bbio = NULL;
}