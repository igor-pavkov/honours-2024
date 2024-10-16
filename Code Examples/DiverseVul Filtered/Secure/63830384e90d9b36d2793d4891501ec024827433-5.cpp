static int ssl3_check_finished(SSL *s)
{
    int ok = 0;

    if (s->version < TLS1_VERSION || !s->tls_session_secret_cb ||
        !s->session->tlsext_tick)
        return 0;

    /* Need to permit this temporarily, in case the next message is Finished. */
    s->s3->flags |= SSL3_FLAGS_CCS_OK;
    /*
     * This function is called when we might get a Certificate message instead,
     * so permit appropriate message length.
     * We ignore the return value as we're only interested in the message type
     * and not its length.
     */
    s->method->ssl_get_message(s,
                               SSL3_ST_CR_CERT_A,
                               SSL3_ST_CR_CERT_B,
                               -1, s->max_cert_list, &ok);
    s->s3->flags &= ~SSL3_FLAGS_CCS_OK;

    if (!ok)
        return -1;

    s->s3->tmp.reuse_message = 1;

    if (s->s3->tmp.message_type == SSL3_MT_FINISHED)
        return 1;

    /* If we're not done, then the CCS arrived early and we should bail. */
    if (s->s3->change_cipher_spec) {
        SSLerr(SSL_F_SSL3_CHECK_FINISHED, SSL_R_CCS_RECEIVED_EARLY);
        ssl3_send_alert(s, SSL3_AL_FATAL, SSL_AD_UNEXPECTED_MESSAGE);
        return -1;
    }

    return 0;
}