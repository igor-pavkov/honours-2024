int tls_collect_extensions(SSL *s, PACKET *packet, unsigned int context,
                           RAW_EXTENSION **res, size_t *len, int init)
{
    PACKET extensions = *packet;
    size_t i = 0;
    size_t num_exts;
    custom_ext_methods *exts = &s->cert->custext;
    RAW_EXTENSION *raw_extensions = NULL;
    const EXTENSION_DEFINITION *thisexd;

    *res = NULL;

    /*
     * Initialise server side custom extensions. Client side is done during
     * construction of extensions for the ClientHello.
     */
    if ((context & SSL_EXT_CLIENT_HELLO) != 0)
        custom_ext_init(&s->cert->custext);

    num_exts = OSSL_NELEM(ext_defs) + (exts != NULL ? exts->meths_count : 0);
    raw_extensions = OPENSSL_zalloc(num_exts * sizeof(*raw_extensions));
    if (raw_extensions == NULL) {
        SSLfatal(s, SSL_AD_INTERNAL_ERROR, SSL_F_TLS_COLLECT_EXTENSIONS,
                 ERR_R_MALLOC_FAILURE);
        return 0;
    }

    i = 0;
    while (PACKET_remaining(&extensions) > 0) {
        unsigned int type, idx;
        PACKET extension;
        RAW_EXTENSION *thisex;

        if (!PACKET_get_net_2(&extensions, &type) ||
            !PACKET_get_length_prefixed_2(&extensions, &extension)) {
            SSLfatal(s, SSL_AD_DECODE_ERROR, SSL_F_TLS_COLLECT_EXTENSIONS,
                     SSL_R_BAD_EXTENSION);
            goto err;
        }
        /*
         * Verify this extension is allowed. We only check duplicates for
         * extensions that we recognise. We also have a special case for the
         * PSK extension, which must be the last one in the ClientHello.
         */
        if (!verify_extension(s, context, type, exts, raw_extensions, &thisex)
                || (thisex != NULL && thisex->present == 1)
                || (type == TLSEXT_TYPE_psk
                    && (context & SSL_EXT_CLIENT_HELLO) != 0
                    && PACKET_remaining(&extensions) != 0)) {
            SSLfatal(s, SSL_AD_ILLEGAL_PARAMETER, SSL_F_TLS_COLLECT_EXTENSIONS,
                     SSL_R_BAD_EXTENSION);
            goto err;
        }
        idx = thisex - raw_extensions;
        /*-
         * Check that we requested this extension (if appropriate). Requests can
         * be sent in the ClientHello and CertificateRequest. Unsolicited
         * extensions can be sent in the NewSessionTicket. We only do this for
         * the built-in extensions. Custom extensions have a different but
         * similar check elsewhere.
         * Special cases:
         * - The HRR cookie extension is unsolicited
         * - The renegotiate extension is unsolicited (the client signals
         *   support via an SCSV)
         * - The signed_certificate_timestamp extension can be provided by a
         * custom extension or by the built-in version. We let the extension
         * itself handle unsolicited response checks.
         */
        if (idx < OSSL_NELEM(ext_defs)
                && (context & (SSL_EXT_CLIENT_HELLO
                               | SSL_EXT_TLS1_3_CERTIFICATE_REQUEST
                               | SSL_EXT_TLS1_3_NEW_SESSION_TICKET)) == 0
                && type != TLSEXT_TYPE_cookie
                && type != TLSEXT_TYPE_renegotiate
                && type != TLSEXT_TYPE_signed_certificate_timestamp
                && (s->ext.extflags[idx] & SSL_EXT_FLAG_SENT) == 0
#ifndef OPENSSL_NO_GOST
                && !((context & SSL_EXT_TLS1_2_SERVER_HELLO) != 0
                     && type == TLSEXT_TYPE_cryptopro_bug)
#endif
								) {
            SSLfatal(s, SSL_AD_UNSUPPORTED_EXTENSION,
                     SSL_F_TLS_COLLECT_EXTENSIONS, SSL_R_UNSOLICITED_EXTENSION);
            goto err;
        }
        if (thisex != NULL) {
            thisex->data = extension;
            thisex->present = 1;
            thisex->type = type;
            thisex->received_order = i++;
            if (s->ext.debug_cb)
                s->ext.debug_cb(s, !s->server, thisex->type,
                                PACKET_data(&thisex->data),
                                PACKET_remaining(&thisex->data),
                                s->ext.debug_arg);
        }
    }

    if (init) {
        /*
         * Initialise all known extensions relevant to this context,
         * whether we have found them or not
         */
        for (thisexd = ext_defs, i = 0; i < OSSL_NELEM(ext_defs);
             i++, thisexd++) {
            if (thisexd->init != NULL && (thisexd->context & context) != 0
                && extension_is_relevant(s, thisexd->context, context)
                && !thisexd->init(s, context)) {
                /* SSLfatal() already called */
                goto err;
            }
        }
    }

    *res = raw_extensions;
    if (len != NULL)
        *len = num_exts;
    return 1;

 err:
    OPENSSL_free(raw_extensions);
    return 0;
}