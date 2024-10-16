int SSL_CTX_use_psk_identity_hint(SSL_CTX *ctx, const char *identity_hint)
{
    if (identity_hint != NULL && strlen(identity_hint) > PSK_MAX_IDENTITY_LEN) {
        SSLerr(SSL_F_SSL_CTX_USE_PSK_IDENTITY_HINT,
               SSL_R_DATA_LENGTH_TOO_LONG);
        return 0;
    }
    if (ctx->psk_identity_hint != NULL)
        OPENSSL_free(ctx->psk_identity_hint);
    if (identity_hint != NULL) {
        ctx->psk_identity_hint = BUF_strdup(identity_hint);
        if (ctx->psk_identity_hint == NULL)
            return 0;
    } else
        ctx->psk_identity_hint = NULL;
    return 1;
}