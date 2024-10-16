static const SIGALG_LOOKUP *tls1_get_legacy_sigalg(const SSL *s, int idx)
{
    if (idx == -1) {
        if (s->server) {
            size_t i;

            /* Work out index corresponding to ciphersuite */
            for (i = 0; i < SSL_PKEY_NUM; i++) {
                const SSL_CERT_LOOKUP *clu = ssl_cert_lookup_by_idx(i);

                if (clu->amask & s->s3.tmp.new_cipher->algorithm_auth) {
                    idx = i;
                    break;
                }
            }

            /*
             * Some GOST ciphersuites allow more than one signature algorithms
             * */
            if (idx == SSL_PKEY_GOST01 && s->s3.tmp.new_cipher->algorithm_auth != SSL_aGOST01) {
                int real_idx;

                for (real_idx = SSL_PKEY_GOST12_512; real_idx >= SSL_PKEY_GOST01;
                     real_idx--) {
                    if (s->cert->pkeys[real_idx].privatekey != NULL) {
                        idx = real_idx;
                        break;
                    }
                }
            }
        } else {
            idx = s->cert->key - s->cert->pkeys;
        }
    }
    if (idx < 0 || idx >= (int)OSSL_NELEM(tls_default_sigalg))
        return NULL;
    if (SSL_USE_SIGALGS(s) || idx != SSL_PKEY_RSA) {
        const SIGALG_LOOKUP *lu = tls1_lookup_sigalg(tls_default_sigalg[idx]);

        if (!tls1_lookup_md(s->ctx, lu, NULL))
            return NULL;
        if (!tls12_sigalg_allowed(s, SSL_SECOP_SIGALG_SUPPORTED, lu))
            return NULL;
        return lu;
    }
    if (!tls12_sigalg_allowed(s, SSL_SECOP_SIGALG_SUPPORTED, &legacy_rsa_sigalg))
        return NULL;
    return &legacy_rsa_sigalg;
}