int extension_is_relevant(SSL *s, unsigned int extctx, unsigned int thisctx)
{
    int is_tls13;

    /*
     * For HRR we haven't selected the version yet but we know it will be
     * TLSv1.3
     */
    if ((thisctx & SSL_EXT_TLS1_3_HELLO_RETRY_REQUEST) != 0)
        is_tls13 = 1;
    else
        is_tls13 = SSL_IS_TLS13(s);

    if ((SSL_IS_DTLS(s)
                && (extctx & SSL_EXT_TLS_IMPLEMENTATION_ONLY) != 0)
            || (s->version == SSL3_VERSION
                    && (extctx & SSL_EXT_SSL3_ALLOWED) == 0)
            /*
             * Note that SSL_IS_TLS13() means "TLS 1.3 has been negotiated",
             * which is never true when generating the ClientHello.
             * However, version negotiation *has* occurred by the time the
             * ClientHello extensions are being parsed.
             * Be careful to allow TLS 1.3-only extensions when generating
             * the ClientHello.
             */
            || (is_tls13 && (extctx & SSL_EXT_TLS1_2_AND_BELOW_ONLY) != 0)
            || (!is_tls13 && (extctx & SSL_EXT_TLS1_3_ONLY) != 0
                && (thisctx & SSL_EXT_CLIENT_HELLO) == 0)
            || (s->server && !is_tls13 && (extctx & SSL_EXT_TLS1_3_ONLY) != 0)
            || (s->hit && (extctx & SSL_EXT_IGNORE_ON_RESUMPTION) != 0))
        return 0;
    return 1;
}