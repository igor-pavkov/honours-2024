static int tls_parse_certificate_authorities(SSL *s, PACKET *pkt,
                                             unsigned int context, X509 *x,
                                             size_t chainidx)
{
    if (!parse_ca_names(s, pkt))
        return 0;
    if (PACKET_remaining(pkt) != 0) {
        SSLfatal(s, SSL_AD_DECODE_ERROR,
                 SSL_F_TLS_PARSE_CERTIFICATE_AUTHORITIES, SSL_R_BAD_EXTENSION);
        return 0;
    }
    return 1;
}