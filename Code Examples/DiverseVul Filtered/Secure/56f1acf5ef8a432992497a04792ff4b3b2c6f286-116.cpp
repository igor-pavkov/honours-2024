int ssl_init_wbio_buffer(SSL *s, int push)
{
    BIO *bbio;

    if (s->bbio == NULL) {
        bbio = BIO_new(BIO_f_buffer());
        if (bbio == NULL)
            return (0);
        s->bbio = bbio;
    } else {
        bbio = s->bbio;
        if (s->bbio == s->wbio)
            s->wbio = BIO_pop(s->wbio);
    }
    (void)BIO_reset(bbio);
/*      if (!BIO_set_write_buffer_size(bbio,16*1024)) */
    if (!BIO_set_read_buffer_size(bbio, 1)) {
        SSLerr(SSL_F_SSL_INIT_WBIO_BUFFER, ERR_R_BUF_LIB);
        return (0);
    }
    if (push) {
        if (s->wbio != bbio)
            s->wbio = BIO_push(bbio, s->wbio);
    } else {
        if (s->wbio == bbio)
            s->wbio = BIO_pop(bbio);
    }
    return (1);
}