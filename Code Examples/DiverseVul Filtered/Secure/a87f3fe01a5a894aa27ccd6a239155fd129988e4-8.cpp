int tls12_check_peer_sigalg(SSL *s, uint16_t sig, EVP_PKEY *pkey)
{
    const uint16_t *sent_sigs;
    const EVP_MD *md = NULL;
    char sigalgstr[2];
    size_t sent_sigslen, i, cidx;
    int pkeyid = -1;
    const SIGALG_LOOKUP *lu;
    int secbits = 0;

    /*
     * TODO(3.0) Remove this when we adapted this function for provider
     * side keys.  We know that EVP_PKEY_get0() downgrades an EVP_PKEY
     * to contain a legacy key.
     *
     * THIS IS TEMPORARY
     */
    EVP_PKEY_get0(pkey);
    if (EVP_PKEY_id(pkey) == EVP_PKEY_NONE)
        return 0;

    pkeyid = EVP_PKEY_id(pkey);
    /* Should never happen */
    if (pkeyid == -1)
        return -1;
    if (SSL_IS_TLS13(s)) {
        /* Disallow DSA for TLS 1.3 */
        if (pkeyid == EVP_PKEY_DSA) {
            SSLfatal(s, SSL_AD_ILLEGAL_PARAMETER, SSL_F_TLS12_CHECK_PEER_SIGALG,
                     SSL_R_WRONG_SIGNATURE_TYPE);
            return 0;
        }
        /* Only allow PSS for TLS 1.3 */
        if (pkeyid == EVP_PKEY_RSA)
            pkeyid = EVP_PKEY_RSA_PSS;
    }
    lu = tls1_lookup_sigalg(sig);
    /*
     * Check sigalgs is known. Disallow SHA1/SHA224 with TLS 1.3. Check key type
     * is consistent with signature: RSA keys can be used for RSA-PSS
     */
    if (lu == NULL
        || (SSL_IS_TLS13(s) && (lu->hash == NID_sha1 || lu->hash == NID_sha224))
        || (pkeyid != lu->sig
        && (lu->sig != EVP_PKEY_RSA_PSS || pkeyid != EVP_PKEY_RSA))) {
        SSLfatal(s, SSL_AD_ILLEGAL_PARAMETER, SSL_F_TLS12_CHECK_PEER_SIGALG,
                 SSL_R_WRONG_SIGNATURE_TYPE);
        return 0;
    }
    /* Check the sigalg is consistent with the key OID */
    if (!ssl_cert_lookup_by_nid(EVP_PKEY_id(pkey), &cidx)
            || lu->sig_idx != (int)cidx) {
        SSLfatal(s, SSL_AD_ILLEGAL_PARAMETER, SSL_F_TLS12_CHECK_PEER_SIGALG,
                 SSL_R_WRONG_SIGNATURE_TYPE);
        return 0;
    }

#ifndef OPENSSL_NO_EC
    if (pkeyid == EVP_PKEY_EC) {

        /* Check point compression is permitted */
        if (!tls1_check_pkey_comp(s, pkey)) {
            SSLfatal(s, SSL_AD_ILLEGAL_PARAMETER,
                     SSL_F_TLS12_CHECK_PEER_SIGALG,
                     SSL_R_ILLEGAL_POINT_COMPRESSION);
            return 0;
        }

        /* For TLS 1.3 or Suite B check curve matches signature algorithm */
        if (SSL_IS_TLS13(s) || tls1_suiteb(s)) {
            int curve = evp_pkey_get_EC_KEY_curve_nid(pkey);

            if (lu->curve != NID_undef && curve != lu->curve) {
                SSLfatal(s, SSL_AD_ILLEGAL_PARAMETER,
                         SSL_F_TLS12_CHECK_PEER_SIGALG, SSL_R_WRONG_CURVE);
                return 0;
            }
        }
        if (!SSL_IS_TLS13(s)) {
            /* Check curve matches extensions */
            if (!tls1_check_group_id(s, tls1_get_group_id(pkey), 1)) {
                SSLfatal(s, SSL_AD_ILLEGAL_PARAMETER,
                         SSL_F_TLS12_CHECK_PEER_SIGALG, SSL_R_WRONG_CURVE);
                return 0;
            }
            if (tls1_suiteb(s)) {
                /* Check sigalg matches a permissible Suite B value */
                if (sig != TLSEXT_SIGALG_ecdsa_secp256r1_sha256
                    && sig != TLSEXT_SIGALG_ecdsa_secp384r1_sha384) {
                    SSLfatal(s, SSL_AD_HANDSHAKE_FAILURE,
                             SSL_F_TLS12_CHECK_PEER_SIGALG,
                             SSL_R_WRONG_SIGNATURE_TYPE);
                    return 0;
                }
            }
        }
    } else if (tls1_suiteb(s)) {
        SSLfatal(s, SSL_AD_HANDSHAKE_FAILURE, SSL_F_TLS12_CHECK_PEER_SIGALG,
                 SSL_R_WRONG_SIGNATURE_TYPE);
        return 0;
    }
#endif

    /* Check signature matches a type we sent */
    sent_sigslen = tls12_get_psigalgs(s, 1, &sent_sigs);
    for (i = 0; i < sent_sigslen; i++, sent_sigs++) {
        if (sig == *sent_sigs)
            break;
    }
    /* Allow fallback to SHA1 if not strict mode */
    if (i == sent_sigslen && (lu->hash != NID_sha1
        || s->cert->cert_flags & SSL_CERT_FLAGS_CHECK_TLS_STRICT)) {
        SSLfatal(s, SSL_AD_HANDSHAKE_FAILURE, SSL_F_TLS12_CHECK_PEER_SIGALG,
                 SSL_R_WRONG_SIGNATURE_TYPE);
        return 0;
    }
    if (!tls1_lookup_md(s->ctx, lu, &md)) {
        SSLfatal(s, SSL_AD_HANDSHAKE_FAILURE, SSL_F_TLS12_CHECK_PEER_SIGALG,
                 SSL_R_UNKNOWN_DIGEST);
        return 0;
    }
    /*
     * Make sure security callback allows algorithm. For historical
     * reasons we have to pass the sigalg as a two byte char array.
     */
    sigalgstr[0] = (sig >> 8) & 0xff;
    sigalgstr[1] = sig & 0xff;
    secbits = sigalg_security_bits(s->ctx, lu);
    if (secbits == 0 ||
        !ssl_security(s, SSL_SECOP_SIGALG_CHECK, secbits,
                      md != NULL ? EVP_MD_type(md) : NID_undef,
                      (void *)sigalgstr)) {
        SSLfatal(s, SSL_AD_HANDSHAKE_FAILURE, SSL_F_TLS12_CHECK_PEER_SIGALG,
                 SSL_R_WRONG_SIGNATURE_TYPE);
        return 0;
    }
    /* Store the sigalg the peer uses */
    s->s3.tmp.peer_sigalg = lu;
    return 1;
}