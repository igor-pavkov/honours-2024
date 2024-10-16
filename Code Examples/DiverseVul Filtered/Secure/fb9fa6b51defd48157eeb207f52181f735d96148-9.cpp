static int final_ems(SSL *s, unsigned int context, int sent)
{
    /*
     * Check extended master secret extension is not dropped on
     * renegotiation.
     */
    if (!(s->s3->flags & TLS1_FLAGS_RECEIVED_EXTMS)
        && (s->s3->flags & TLS1_FLAGS_REQUIRED_EXTMS)) {
        SSLfatal(s, SSL_AD_HANDSHAKE_FAILURE, SSL_F_FINAL_EMS,
                 SSL_R_INCONSISTENT_EXTMS);
        return 0;
    }
    if (!s->server && s->hit) {
        /*
         * Check extended master secret extension is consistent with
         * original session.
         */
        if (!(s->s3->flags & TLS1_FLAGS_RECEIVED_EXTMS) !=
            !(s->session->flags & SSL_SESS_FLAG_EXTMS)) {
            SSLfatal(s, SSL_AD_HANDSHAKE_FAILURE, SSL_F_FINAL_EMS,
                     SSL_R_INCONSISTENT_EXTMS);
            return 0;
        }
    }

    return 1;
}