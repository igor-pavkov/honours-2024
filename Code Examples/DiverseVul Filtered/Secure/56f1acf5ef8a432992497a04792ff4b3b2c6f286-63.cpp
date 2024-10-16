int SSL_use_psk_identity_hint(SSL *s, const char *identity_hint)
{
    if (s == NULL)
        return 0;

    if (s->session == NULL)
        return 1;               /* session not created yet, ignored */

    if (identity_hint != NULL && strlen(identity_hint) > PSK_MAX_IDENTITY_LEN) {
        SSLerr(SSL_F_SSL_USE_PSK_IDENTITY_HINT, SSL_R_DATA_LENGTH_TOO_LONG);
        return 0;
    }
    if (s->session->psk_identity_hint != NULL)
        OPENSSL_free(s->session->psk_identity_hint);
    if (identity_hint != NULL) {
        s->session->psk_identity_hint = BUF_strdup(identity_hint);
        if (s->session->psk_identity_hint == NULL)
            return 0;
    } else
        s->session->psk_identity_hint = NULL;
    return 1;
}