int ssl_check_srvr_ecc_cert_and_alg(X509 *x, SSL *s)
{
    unsigned long alg_k, alg_a;
    EVP_PKEY *pkey = NULL;
    int keysize = 0;
    int signature_nid = 0, md_nid = 0, pk_nid = 0;
    const SSL_CIPHER *cs = s->s3->tmp.new_cipher;

    alg_k = cs->algorithm_mkey;
    alg_a = cs->algorithm_auth;

    if (SSL_C_IS_EXPORT(cs)) {
        /* ECDH key length in export ciphers must be <= 163 bits */
        pkey = X509_get_pubkey(x);
        if (pkey == NULL)
            return 0;
        keysize = EVP_PKEY_bits(pkey);
        EVP_PKEY_free(pkey);
        if (keysize > 163)
            return 0;
    }

    /* This call populates the ex_flags field correctly */
    X509_check_purpose(x, -1, 0);
    if ((x->sig_alg) && (x->sig_alg->algorithm)) {
        signature_nid = OBJ_obj2nid(x->sig_alg->algorithm);
        OBJ_find_sigid_algs(signature_nid, &md_nid, &pk_nid);
    }
    if (alg_k & SSL_kECDHe || alg_k & SSL_kECDHr) {
        /* key usage, if present, must allow key agreement */
        if (ku_reject(x, X509v3_KU_KEY_AGREEMENT)) {
            SSLerr(SSL_F_SSL_CHECK_SRVR_ECC_CERT_AND_ALG,
                   SSL_R_ECC_CERT_NOT_FOR_KEY_AGREEMENT);
            return 0;
        }
        if ((alg_k & SSL_kECDHe) && TLS1_get_version(s) < TLS1_2_VERSION) {
            /* signature alg must be ECDSA */
            if (pk_nid != NID_X9_62_id_ecPublicKey) {
                SSLerr(SSL_F_SSL_CHECK_SRVR_ECC_CERT_AND_ALG,
                       SSL_R_ECC_CERT_SHOULD_HAVE_SHA1_SIGNATURE);
                return 0;
            }
        }
        if ((alg_k & SSL_kECDHr) && TLS1_get_version(s) < TLS1_2_VERSION) {
            /* signature alg must be RSA */

            if (pk_nid != NID_rsaEncryption && pk_nid != NID_rsa) {
                SSLerr(SSL_F_SSL_CHECK_SRVR_ECC_CERT_AND_ALG,
                       SSL_R_ECC_CERT_SHOULD_HAVE_RSA_SIGNATURE);
                return 0;
            }
        }
    }
    if (alg_a & SSL_aECDSA) {
        /* key usage, if present, must allow signing */
        if (ku_reject(x, X509v3_KU_DIGITAL_SIGNATURE)) {
            SSLerr(SSL_F_SSL_CHECK_SRVR_ECC_CERT_AND_ALG,
                   SSL_R_ECC_CERT_NOT_FOR_SIGNING);
            return 0;
        }
    }

    return 1;                   /* all checks are ok */
}