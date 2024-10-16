CERT_PKEY *ssl_get_server_send_pkey(const SSL *s)
{
    unsigned long alg_k, alg_a;
    CERT *c;
    int i;

    c = s->cert;
    ssl_set_cert_masks(c, s->s3->tmp.new_cipher);

    alg_k = s->s3->tmp.new_cipher->algorithm_mkey;
    alg_a = s->s3->tmp.new_cipher->algorithm_auth;

    if (alg_k & (SSL_kECDHr | SSL_kECDHe)) {
        /*
         * we don't need to look at SSL_kEECDH since no certificate is needed
         * for anon ECDH and for authenticated EECDH, the check for the auth
         * algorithm will set i correctly NOTE: For ECDH-RSA, we need an ECC
         * not an RSA cert but for EECDH-RSA we need an RSA cert. Placing the
         * checks for SSL_kECDH before RSA checks ensures the correct cert is
         * chosen.
         */
        i = SSL_PKEY_ECC;
    } else if (alg_a & SSL_aECDSA) {
        i = SSL_PKEY_ECC;
    } else if (alg_k & SSL_kDHr)
        i = SSL_PKEY_DH_RSA;
    else if (alg_k & SSL_kDHd)
        i = SSL_PKEY_DH_DSA;
    else if (alg_a & SSL_aDSS)
        i = SSL_PKEY_DSA_SIGN;
    else if (alg_a & SSL_aRSA) {
        if (c->pkeys[SSL_PKEY_RSA_ENC].x509 == NULL)
            i = SSL_PKEY_RSA_SIGN;
        else
            i = SSL_PKEY_RSA_ENC;
    } else if (alg_a & SSL_aKRB5) {
        /* VRS something else here? */
        return (NULL);
    } else if (alg_a & SSL_aGOST94)
        i = SSL_PKEY_GOST94;
    else if (alg_a & SSL_aGOST01)
        i = SSL_PKEY_GOST01;
    else {                      /* if (alg_a & SSL_aNULL) */

        SSLerr(SSL_F_SSL_GET_SERVER_SEND_PKEY, ERR_R_INTERNAL_ERROR);
        return (NULL);
    }

    return c->pkeys + i;
}