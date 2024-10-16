static int ssl_set_version(SSL *s)
{
    unsigned long mask, options = s->options;

    if (s->method->version == TLS_ANY_VERSION) {
        /*
         * SSL_OP_NO_X disables all protocols above X *if* there are
         * some protocols below X enabled. This is required in order
         * to maintain "version capability" vector contiguous. So
         * that if application wants to disable TLS1.0 in favour of
         * TLS1>=1, it would be insufficient to pass SSL_NO_TLSv1, the
         * answer is SSL_OP_NO_TLSv1|SSL_OP_NO_SSLv3.
         */
        mask = SSL_OP_NO_TLSv1_1 | SSL_OP_NO_TLSv1
#if !defined(OPENSSL_NO_SSL3)
            | SSL_OP_NO_SSLv3
#endif
            ;
#if !defined(OPENSSL_NO_TLS1_2_CLIENT)
        if (options & SSL_OP_NO_TLSv1_2) {
            if ((options & mask) != mask) {
                s->version = TLS1_1_VERSION;
            } else {
                SSLerr(SSL_F_SSL_SET_VERSION, SSL_R_NO_PROTOCOLS_AVAILABLE);
                return 0;
            }
        } else {
            s->version = TLS1_2_VERSION;
        }
#else
        if ((options & mask) == mask) {
            SSLerr(SSL_F_SSL_SET_VERSION, SSL_R_NO_PROTOCOLS_AVAILABLE);
            return 0;
        }
        s->version = TLS1_1_VERSION;
#endif

        mask &= ~SSL_OP_NO_TLSv1_1;
        if ((options & SSL_OP_NO_TLSv1_1) && (options & mask) != mask)
            s->version = TLS1_VERSION;
        mask &= ~SSL_OP_NO_TLSv1;
#if !defined(OPENSSL_NO_SSL3)
        if ((options & SSL_OP_NO_TLSv1) && (options & mask) != mask)
            s->version = SSL3_VERSION;
#endif

        if (s->version != TLS1_2_VERSION && tls1_suiteb(s)) {
            SSLerr(SSL_F_SSL_SET_VERSION,
                   SSL_R_ONLY_TLS_1_2_ALLOWED_IN_SUITEB_MODE);
            return 0;
        }

        if (s->version == SSL3_VERSION && FIPS_mode()) {
            SSLerr(SSL_F_SSL_SET_VERSION, SSL_R_ONLY_TLS_ALLOWED_IN_FIPS_MODE);
            return 0;
        }

    } else if (s->method->version == DTLS_ANY_VERSION) {
        /* Determine which DTLS version to use */
        /* If DTLS 1.2 disabled correct the version number */
        if (options & SSL_OP_NO_DTLSv1_2) {
            if (tls1_suiteb(s)) {
                SSLerr(SSL_F_SSL_SET_VERSION,
                       SSL_R_ONLY_DTLS_1_2_ALLOWED_IN_SUITEB_MODE);
                return 0;
            }
            /*
             * Disabling all versions is silly: return an error.
             */
            if (options & SSL_OP_NO_DTLSv1) {
                SSLerr(SSL_F_SSL_SET_VERSION, SSL_R_WRONG_SSL_VERSION);
                return 0;
            }
            /*
             * Update method so we don't use any DTLS 1.2 features.
             */
            s->method = DTLSv1_client_method();
            s->version = DTLS1_VERSION;
        } else {
            /*
             * We only support one version: update method
             */
            if (options & SSL_OP_NO_DTLSv1)
                s->method = DTLSv1_2_client_method();
            s->version = DTLS1_2_VERSION;
        }
    }

    s->client_version = s->version;

    return 1;
}