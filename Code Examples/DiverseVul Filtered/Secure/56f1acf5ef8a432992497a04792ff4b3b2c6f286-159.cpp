EVP_PKEY *ssl_get_sign_pkey(SSL *s, const SSL_CIPHER *cipher,
                            const EVP_MD **pmd)
{
    unsigned long alg_a;
    CERT *c;
    int idx = -1;

    alg_a = cipher->algorithm_auth;
    c = s->cert;

    if ((alg_a & SSL_aDSS) &&
        (c->pkeys[SSL_PKEY_DSA_SIGN].privatekey != NULL))
        idx = SSL_PKEY_DSA_SIGN;
    else if (alg_a & SSL_aRSA) {
        if (c->pkeys[SSL_PKEY_RSA_SIGN].privatekey != NULL)
            idx = SSL_PKEY_RSA_SIGN;
        else if (c->pkeys[SSL_PKEY_RSA_ENC].privatekey != NULL)
            idx = SSL_PKEY_RSA_ENC;
    } else if ((alg_a & SSL_aECDSA) &&
               (c->pkeys[SSL_PKEY_ECC].privatekey != NULL))
        idx = SSL_PKEY_ECC;
    if (idx == -1) {
        SSLerr(SSL_F_SSL_GET_SIGN_PKEY, ERR_R_INTERNAL_ERROR);
        return (NULL);
    }
    if (pmd)
        *pmd = c->pkeys[idx].digest;
    return c->pkeys[idx].privatekey;
}