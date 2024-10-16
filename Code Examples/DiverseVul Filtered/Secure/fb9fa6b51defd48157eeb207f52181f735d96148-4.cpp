static EXT_RETURN tls_construct_certificate_authorities(SSL *s, WPACKET *pkt,
                                                        unsigned int context,
                                                        X509 *x,
                                                        size_t chainidx)
{
    const STACK_OF(X509_NAME) *ca_sk = get_ca_names(s);

    if (ca_sk == NULL || sk_X509_NAME_num(ca_sk) == 0)
        return EXT_RETURN_NOT_SENT;

    if (!WPACKET_put_bytes_u16(pkt, TLSEXT_TYPE_certificate_authorities)
        || !WPACKET_start_sub_packet_u16(pkt)) {
        SSLfatal(s, SSL_AD_INTERNAL_ERROR,
                 SSL_F_TLS_CONSTRUCT_CERTIFICATE_AUTHORITIES,
               ERR_R_INTERNAL_ERROR);
        return EXT_RETURN_FAIL;
    }

    if (!construct_ca_names(s, ca_sk, pkt)) {
        /* SSLfatal() already called */
        return EXT_RETURN_FAIL;
    }

    if (!WPACKET_close(pkt)) {
        SSLfatal(s, SSL_AD_INTERNAL_ERROR,
                 SSL_F_TLS_CONSTRUCT_CERTIFICATE_AUTHORITIES,
                 ERR_R_INTERNAL_ERROR);
        return EXT_RETURN_FAIL;
    }

    return EXT_RETURN_SENT;
}