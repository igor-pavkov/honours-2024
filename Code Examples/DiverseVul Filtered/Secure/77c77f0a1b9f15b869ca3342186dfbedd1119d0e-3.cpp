int ssl3_dispatch_alert(SSL *s)
{
    int i, j;
    void (*cb) (const SSL *ssl, int type, int val) = NULL;

    s->s3->alert_dispatch = 0;
    i = do_ssl3_write(s, SSL3_RT_ALERT, &s->s3->send_alert[0], 2, 0);
    if (i <= 0) {
        s->s3->alert_dispatch = 1;
    } else {
        /*
         * Alert sent to BIO.  If it is important, flush it now. If the
         * message does not get sent due to non-blocking IO, we will not
         * worry too much.
         */
        if (s->s3->send_alert[0] == SSL3_AL_FATAL)
            (void)BIO_flush(s->wbio);

        if (s->msg_callback)
            s->msg_callback(1, s->version, SSL3_RT_ALERT, s->s3->send_alert,
                            2, s, s->msg_callback_arg);

        if (s->info_callback != NULL)
            cb = s->info_callback;
        else if (s->ctx->info_callback != NULL)
            cb = s->ctx->info_callback;

        if (cb != NULL) {
            j = (s->s3->send_alert[0] << 8) | s->s3->send_alert[1];
            cb(s, SSL_CB_WRITE_ALERT, j);
        }
    }
    return (i);
}