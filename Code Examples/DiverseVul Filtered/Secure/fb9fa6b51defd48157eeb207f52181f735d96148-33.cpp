static int final_early_data(SSL *s, unsigned int context, int sent)
{
    if (!sent)
        return 1;

    if (!s->server) {
        if (context == SSL_EXT_TLS1_3_ENCRYPTED_EXTENSIONS
                && sent
                && !s->ext.early_data_ok) {
            /*
             * If we get here then the server accepted our early_data but we
             * later realised that it shouldn't have done (e.g. inconsistent
             * ALPN)
             */
            SSLfatal(s, SSL_AD_ILLEGAL_PARAMETER, SSL_F_FINAL_EARLY_DATA,
                     SSL_R_BAD_EARLY_DATA);
            return 0;
        }

        return 1;
    }

    if (s->max_early_data == 0
            || !s->hit
            || s->early_data_state != SSL_EARLY_DATA_ACCEPTING
            || !s->ext.early_data_ok
            || s->hello_retry_request != SSL_HRR_NONE
            || (s->allow_early_data_cb != NULL
                && !s->allow_early_data_cb(s,
                                         s->allow_early_data_cb_data))) {
        s->ext.early_data = SSL_EARLY_DATA_REJECTED;
    } else {
        s->ext.early_data = SSL_EARLY_DATA_ACCEPTED;

        if (!tls13_change_cipher_state(s,
                    SSL3_CC_EARLY | SSL3_CHANGE_CIPHER_SERVER_READ)) {
            /* SSLfatal() already called */
            return 0;
        }
    }

    return 1;
}