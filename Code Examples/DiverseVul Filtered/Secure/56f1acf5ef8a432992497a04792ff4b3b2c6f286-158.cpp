int SSL_check_private_key(const SSL *ssl)
{
    if (ssl == NULL) {
        SSLerr(SSL_F_SSL_CHECK_PRIVATE_KEY, ERR_R_PASSED_NULL_PARAMETER);
        return (0);
    }
    if (ssl->cert == NULL) {
        SSLerr(SSL_F_SSL_CHECK_PRIVATE_KEY, SSL_R_NO_CERTIFICATE_ASSIGNED);
        return 0;
    }
    if (ssl->cert->key->x509 == NULL) {
        SSLerr(SSL_F_SSL_CHECK_PRIVATE_KEY, SSL_R_NO_CERTIFICATE_ASSIGNED);
        return (0);
    }
    if (ssl->cert->key->privatekey == NULL) {
        SSLerr(SSL_F_SSL_CHECK_PRIVATE_KEY, SSL_R_NO_PRIVATE_KEY_ASSIGNED);
        return (0);
    }
    return (X509_check_private_key(ssl->cert->key->x509,
                                   ssl->cert->key->privatekey));
}