int ssl_cipher_disabled(const SSL *s, const SSL_CIPHER *c, int op, int ecdhe)
{
    if (c->algorithm_mkey & s->s3.tmp.mask_k
        || c->algorithm_auth & s->s3.tmp.mask_a)
        return 1;
    if (s->s3.tmp.max_ver == 0)
        return 1;
    if (!SSL_IS_DTLS(s)) {
        int min_tls = c->min_tls;

        /*
         * For historical reasons we will allow ECHDE to be selected by a server
         * in SSLv3 if we are a client
         */
        if (min_tls == TLS1_VERSION && ecdhe
                && (c->algorithm_mkey & (SSL_kECDHE | SSL_kECDHEPSK)) != 0)
            min_tls = SSL3_VERSION;

        if ((min_tls > s->s3.tmp.max_ver) || (c->max_tls < s->s3.tmp.min_ver))
            return 1;
    }
    if (SSL_IS_DTLS(s) && (DTLS_VERSION_GT(c->min_dtls, s->s3.tmp.max_ver)
                           || DTLS_VERSION_LT(c->max_dtls, s->s3.tmp.min_ver)))
        return 1;

    return !ssl_security(s, op, c->strength_bits, 0, (void *)c);
}