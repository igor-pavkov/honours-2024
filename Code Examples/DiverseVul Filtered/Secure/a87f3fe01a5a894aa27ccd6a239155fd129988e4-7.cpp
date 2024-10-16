static int tls12_sigalg_allowed(const SSL *s, int op, const SIGALG_LOOKUP *lu)
{
    unsigned char sigalgstr[2];
    int secbits;

    /* See if sigalgs is recognised and if hash is enabled */
    if (!tls1_lookup_md(s->ctx, lu, NULL))
        return 0;
    /* DSA is not allowed in TLS 1.3 */
    if (SSL_IS_TLS13(s) && lu->sig == EVP_PKEY_DSA)
        return 0;
    /* TODO(OpenSSL1.2) fully axe DSA/etc. in ClientHello per TLS 1.3 spec */
    if (!s->server && !SSL_IS_DTLS(s) && s->s3.tmp.min_ver >= TLS1_3_VERSION
        && (lu->sig == EVP_PKEY_DSA || lu->hash_idx == SSL_MD_SHA1_IDX
            || lu->hash_idx == SSL_MD_MD5_IDX
            || lu->hash_idx == SSL_MD_SHA224_IDX))
        return 0;

    /* See if public key algorithm allowed */
    if (ssl_cert_is_disabled(lu->sig_idx))
        return 0;

    if (lu->sig == NID_id_GostR3410_2012_256
            || lu->sig == NID_id_GostR3410_2012_512
            || lu->sig == NID_id_GostR3410_2001) {
        /* We never allow GOST sig algs on the server with TLSv1.3 */
        if (s->server && SSL_IS_TLS13(s))
            return 0;
        if (!s->server
                && s->method->version == TLS_ANY_VERSION
                && s->s3.tmp.max_ver >= TLS1_3_VERSION) {
            int i, num;
            STACK_OF(SSL_CIPHER) *sk;

            /*
             * We're a client that could negotiate TLSv1.3. We only allow GOST
             * sig algs if we could negotiate TLSv1.2 or below and we have GOST
             * ciphersuites enabled.
             */

            if (s->s3.tmp.min_ver >= TLS1_3_VERSION)
                return 0;

            sk = SSL_get_ciphers(s);
            num = sk != NULL ? sk_SSL_CIPHER_num(sk) : 0;
            for (i = 0; i < num; i++) {
                const SSL_CIPHER *c;

                c = sk_SSL_CIPHER_value(sk, i);
                /* Skip disabled ciphers */
                if (ssl_cipher_disabled(s, c, SSL_SECOP_CIPHER_SUPPORTED, 0))
                    continue;

                if ((c->algorithm_mkey & SSL_kGOST) != 0)
                    break;
            }
            if (i == num)
                return 0;
        }
    }

    /* Finally see if security callback allows it */
    secbits = sigalg_security_bits(s->ctx, lu);
    sigalgstr[0] = (lu->sigalg >> 8) & 0xff;
    sigalgstr[1] = lu->sigalg & 0xff;
    return ssl_security(s, op, secbits, lu->hash, (void *)sigalgstr);
}