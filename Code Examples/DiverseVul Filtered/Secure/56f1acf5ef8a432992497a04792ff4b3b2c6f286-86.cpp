int SSL_CTX_check_private_key(const SSL_CTX *ctx)
{
    if ((ctx == NULL) ||
        (ctx->cert == NULL) || (ctx->cert->key->x509 == NULL)) {
        SSLerr(SSL_F_SSL_CTX_CHECK_PRIVATE_KEY,
               SSL_R_NO_CERTIFICATE_ASSIGNED);
        return (0);
    }
    if (ctx->cert->key->privatekey == NULL) {
        SSLerr(SSL_F_SSL_CTX_CHECK_PRIVATE_KEY,
               SSL_R_NO_PRIVATE_KEY_ASSIGNED);
        return (0);
    }
    return (X509_check_private_key
            (ctx->cert->key->x509, ctx->cert->key->privatekey));
}