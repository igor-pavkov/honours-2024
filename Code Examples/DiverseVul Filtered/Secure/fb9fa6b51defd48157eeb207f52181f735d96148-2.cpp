static int init_status_request(SSL *s, unsigned int context)
{
    if (s->server) {
        s->ext.status_type = TLSEXT_STATUSTYPE_nothing;
    } else {
        /*
         * Ensure we get sensible values passed to tlsext_status_cb in the event
         * that we don't receive a status message
         */
        OPENSSL_free(s->ext.ocsp.resp);
        s->ext.ocsp.resp = NULL;
        s->ext.ocsp.resp_len = 0;
    }

    return 1;
}