void SSL_set_bio(SSL *s, BIO *rbio, BIO *wbio)
{
    /*
     * If the output buffering BIO is still in place, remove it
     */
    if (s->bbio != NULL) {
        if (s->wbio == s->bbio) {
            s->wbio = s->wbio->next_bio;
            s->bbio->next_bio = NULL;
        }
    }
    if ((s->rbio != NULL) && (s->rbio != rbio))
        BIO_free_all(s->rbio);
    if ((s->wbio != NULL) && (s->wbio != wbio) && (s->rbio != s->wbio))
        BIO_free_all(s->wbio);
    s->rbio = rbio;
    s->wbio = wbio;
}