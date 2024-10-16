int tls_choose_sigalg(SSL *s, int fatalerrs)
{
    const SIGALG_LOOKUP *lu = NULL;
    int sig_idx = -1;

    s->s3.tmp.cert = NULL;
    s->s3.tmp.sigalg = NULL;

    if (SSL_IS_TLS13(s)) {
        lu = find_sig_alg(s, NULL, NULL);
        if (lu == NULL) {
            if (!fatalerrs)
                return 1;
            SSLfatal(s, SSL_AD_HANDSHAKE_FAILURE, SSL_F_TLS_CHOOSE_SIGALG,
                     SSL_R_NO_SUITABLE_SIGNATURE_ALGORITHM);
            return 0;
        }
    } else {
        /* If ciphersuite doesn't require a cert nothing to do */
        if (!(s->s3.tmp.new_cipher->algorithm_auth & SSL_aCERT))
            return 1;
        if (!s->server && !ssl_has_cert(s, s->cert->key - s->cert->pkeys))
                return 1;

        if (SSL_USE_SIGALGS(s)) {
            size_t i;
            if (s->s3.tmp.peer_sigalgs != NULL) {
#ifndef OPENSSL_NO_EC
                int curve = -1;

                /* For Suite B need to match signature algorithm to curve */
                if (tls1_suiteb(s))
                    curve =
                        evp_pkey_get_EC_KEY_curve_nid(s->cert->pkeys[SSL_PKEY_ECC]
                                                      .privatekey);
#endif

                /*
                 * Find highest preference signature algorithm matching
                 * cert type
                 */
                for (i = 0; i < s->shared_sigalgslen; i++) {
                    lu = s->shared_sigalgs[i];

                    if (s->server) {
                        if ((sig_idx = tls12_get_cert_sigalg_idx(s, lu)) == -1)
                            continue;
                    } else {
                        int cc_idx = s->cert->key - s->cert->pkeys;

                        sig_idx = lu->sig_idx;
                        if (cc_idx != sig_idx)
                            continue;
                    }
                    /* Check that we have a cert, and sig_algs_cert */
                    if (!has_usable_cert(s, lu, sig_idx))
                        continue;
                    if (lu->sig == EVP_PKEY_RSA_PSS) {
                        /* validate that key is large enough for the signature algorithm */
                        EVP_PKEY *pkey = s->cert->pkeys[sig_idx].privatekey;

                        if (!rsa_pss_check_min_key_size(s->ctx, pkey, lu))
                            continue;
                    }
#ifndef OPENSSL_NO_EC
                    if (curve == -1 || lu->curve == curve)
#endif
                        break;
                }
#ifndef OPENSSL_NO_GOST
                /*
                 * Some Windows-based implementations do not send GOST algorithms indication
                 * in supported_algorithms extension, so when we have GOST-based ciphersuite,
                 * we have to assume GOST support.
                 */
                if (i == s->shared_sigalgslen && s->s3.tmp.new_cipher->algorithm_auth & (SSL_aGOST01 | SSL_aGOST12)) {
                  if ((lu = tls1_get_legacy_sigalg(s, -1)) == NULL) {
                    if (!fatalerrs)
                      return 1;
                    SSLfatal(s, SSL_AD_HANDSHAKE_FAILURE,
                             SSL_F_TLS_CHOOSE_SIGALG,
                             SSL_R_NO_SUITABLE_SIGNATURE_ALGORITHM);
                    return 0;
                  } else {
                    i = 0;
                    sig_idx = lu->sig_idx;
                  }
                }
#endif
                if (i == s->shared_sigalgslen) {
                    if (!fatalerrs)
                        return 1;
                    SSLfatal(s, SSL_AD_HANDSHAKE_FAILURE,
                             SSL_F_TLS_CHOOSE_SIGALG,
                             SSL_R_NO_SUITABLE_SIGNATURE_ALGORITHM);
                    return 0;
                }
            } else {
                /*
                 * If we have no sigalg use defaults
                 */
                const uint16_t *sent_sigs;
                size_t sent_sigslen;

                if ((lu = tls1_get_legacy_sigalg(s, -1)) == NULL) {
                    if (!fatalerrs)
                        return 1;
                    SSLfatal(s, SSL_AD_INTERNAL_ERROR, SSL_F_TLS_CHOOSE_SIGALG,
                             SSL_R_NO_SUITABLE_SIGNATURE_ALGORITHM);
                    return 0;
                }

                /* Check signature matches a type we sent */
                sent_sigslen = tls12_get_psigalgs(s, 1, &sent_sigs);
                for (i = 0; i < sent_sigslen; i++, sent_sigs++) {
                    if (lu->sigalg == *sent_sigs
                            && has_usable_cert(s, lu, lu->sig_idx))
                        break;
                }
                if (i == sent_sigslen) {
                    if (!fatalerrs)
                        return 1;
                    SSLfatal(s, SSL_AD_ILLEGAL_PARAMETER,
                             SSL_F_TLS_CHOOSE_SIGALG,
                             SSL_R_WRONG_SIGNATURE_TYPE);
                    return 0;
                }
            }
        } else {
            if ((lu = tls1_get_legacy_sigalg(s, -1)) == NULL) {
                if (!fatalerrs)
                    return 1;
                SSLfatal(s, SSL_AD_INTERNAL_ERROR, SSL_F_TLS_CHOOSE_SIGALG,
                         SSL_R_NO_SUITABLE_SIGNATURE_ALGORITHM);
                return 0;
            }
        }
    }
    if (sig_idx == -1)
        sig_idx = lu->sig_idx;
    s->s3.tmp.cert = &s->cert->pkeys[sig_idx];
    s->cert->key = s->s3.tmp.cert;
    s->s3.tmp.sigalg = lu;
    return 1;
}