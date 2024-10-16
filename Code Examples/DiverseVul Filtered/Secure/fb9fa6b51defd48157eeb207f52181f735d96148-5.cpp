int tls_construct_extensions(SSL *s, WPACKET *pkt, unsigned int context,
                             X509 *x, size_t chainidx)
{
    size_t i;
    int min_version, max_version = 0, reason;
    const EXTENSION_DEFINITION *thisexd;

    if (!WPACKET_start_sub_packet_u16(pkt)
               /*
                * If extensions are of zero length then we don't even add the
                * extensions length bytes to a ClientHello/ServerHello
                * (for non-TLSv1.3).
                */
            || ((context &
                 (SSL_EXT_CLIENT_HELLO | SSL_EXT_TLS1_2_SERVER_HELLO)) != 0
                && !WPACKET_set_flags(pkt,
                                     WPACKET_FLAGS_ABANDON_ON_ZERO_LENGTH))) {
        SSLfatal(s, SSL_AD_INTERNAL_ERROR, SSL_F_TLS_CONSTRUCT_EXTENSIONS,
                 ERR_R_INTERNAL_ERROR);
        return 0;
    }

    if ((context & SSL_EXT_CLIENT_HELLO) != 0) {
        reason = ssl_get_min_max_version(s, &min_version, &max_version, NULL);
        if (reason != 0) {
            SSLfatal(s, SSL_AD_INTERNAL_ERROR, SSL_F_TLS_CONSTRUCT_EXTENSIONS,
                     reason);
            return 0;
        }
    }

    /* Add custom extensions first */
    if ((context & SSL_EXT_CLIENT_HELLO) != 0) {
        /* On the server side with initialise during ClientHello parsing */
        custom_ext_init(&s->cert->custext);
    }
    if (!custom_ext_add(s, context, pkt, x, chainidx, max_version)) {
        /* SSLfatal() already called */
        return 0;
    }

    for (i = 0, thisexd = ext_defs; i < OSSL_NELEM(ext_defs); i++, thisexd++) {
        EXT_RETURN (*construct)(SSL *s, WPACKET *pkt, unsigned int context,
                                X509 *x, size_t chainidx);
        EXT_RETURN ret;

        /* Skip if not relevant for our context */
        if (!should_add_extension(s, thisexd->context, context, max_version))
            continue;

        construct = s->server ? thisexd->construct_stoc
                              : thisexd->construct_ctos;

        if (construct == NULL)
            continue;

        ret = construct(s, pkt, context, x, chainidx);
        if (ret == EXT_RETURN_FAIL) {
            /* SSLfatal() already called */
            return 0;
        }
        if (ret == EXT_RETURN_SENT
                && (context & (SSL_EXT_CLIENT_HELLO
                               | SSL_EXT_TLS1_3_CERTIFICATE_REQUEST
                               | SSL_EXT_TLS1_3_NEW_SESSION_TICKET)) != 0)
            s->ext.extflags[i] |= SSL_EXT_FLAG_SENT;
    }

    if (!WPACKET_close(pkt)) {
        SSLfatal(s, SSL_AD_INTERNAL_ERROR, SSL_F_TLS_CONSTRUCT_EXTENSIONS,
                 ERR_R_INTERNAL_ERROR);
        return 0;
    }

    return 1;
}