static int final_renegotiate(SSL *s, unsigned int context, int sent)
{
    if (!s->server) {
        /*
         * Check if we can connect to a server that doesn't support safe
         * renegotiation
         */
        if (!(s->options & SSL_OP_LEGACY_SERVER_CONNECT)
                && !(s->options & SSL_OP_ALLOW_UNSAFE_LEGACY_RENEGOTIATION)
                && !sent) {
            SSLfatal(s, SSL_AD_HANDSHAKE_FAILURE, SSL_F_FINAL_RENEGOTIATE,
                     SSL_R_UNSAFE_LEGACY_RENEGOTIATION_DISABLED);
            return 0;
        }

        return 1;
    }

    /* Need RI if renegotiating */
    if (s->renegotiate
            && !(s->options & SSL_OP_ALLOW_UNSAFE_LEGACY_RENEGOTIATION)
            && !sent) {
        SSLfatal(s, SSL_AD_HANDSHAKE_FAILURE, SSL_F_FINAL_RENEGOTIATE,
                 SSL_R_UNSAFE_LEGACY_RENEGOTIATION_DISABLED);
        return 0;
    }


    return 1;
}