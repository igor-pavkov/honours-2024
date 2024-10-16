static int validate_context(SSL *s, unsigned int extctx, unsigned int thisctx)
{
    /* Check we're allowed to use this extension in this context */
    if ((thisctx & extctx) == 0)
        return 0;

    if (SSL_IS_DTLS(s)) {
        if ((extctx & SSL_EXT_TLS_ONLY) != 0)
            return 0;
    } else if ((extctx & SSL_EXT_DTLS_ONLY) != 0) {
        return 0;
    }

    return 1;
}