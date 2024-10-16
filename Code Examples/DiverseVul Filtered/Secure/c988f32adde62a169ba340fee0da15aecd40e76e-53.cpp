int ssl_check_cert_usage( const x509_crt *cert,
                          const ssl_ciphersuite_t *ciphersuite,
                          int cert_endpoint,
                          int *flags )
{
    int ret = 0;
#if defined(POLARSSL_X509_CHECK_KEY_USAGE)
    int usage = 0;
#endif
#if defined(POLARSSL_X509_CHECK_EXTENDED_KEY_USAGE)
    const char *ext_oid;
    size_t ext_len;
#endif

#if !defined(POLARSSL_X509_CHECK_KEY_USAGE) &&          \
    !defined(POLARSSL_X509_CHECK_EXTENDED_KEY_USAGE)
    ((void) cert);
    ((void) cert_endpoint);
    ((void) flags);
#endif

#if defined(POLARSSL_X509_CHECK_KEY_USAGE)
    if( cert_endpoint == SSL_IS_SERVER )
    {
        /* Server part of the key exchange */
        switch( ciphersuite->key_exchange )
        {
            case POLARSSL_KEY_EXCHANGE_RSA:
            case POLARSSL_KEY_EXCHANGE_RSA_PSK:
                usage = KU_KEY_ENCIPHERMENT;
                break;

            case POLARSSL_KEY_EXCHANGE_DHE_RSA:
            case POLARSSL_KEY_EXCHANGE_ECDHE_RSA:
            case POLARSSL_KEY_EXCHANGE_ECDHE_ECDSA:
                usage = KU_DIGITAL_SIGNATURE;
                break;

            case POLARSSL_KEY_EXCHANGE_ECDH_RSA:
            case POLARSSL_KEY_EXCHANGE_ECDH_ECDSA:
                usage = KU_KEY_AGREEMENT;
                break;

            /* Don't use default: we want warnings when adding new values */
            case POLARSSL_KEY_EXCHANGE_NONE:
            case POLARSSL_KEY_EXCHANGE_PSK:
            case POLARSSL_KEY_EXCHANGE_DHE_PSK:
            case POLARSSL_KEY_EXCHANGE_ECDHE_PSK:
                usage = 0;
        }
    }
    else
    {
        /* Client auth: we only implement rsa_sign and ecdsa_sign for now */
        usage = KU_DIGITAL_SIGNATURE;
    }

    if( x509_crt_check_key_usage( cert, usage ) != 0 )
    {
        *flags |= BADCERT_KEY_USAGE;
        ret = -1;
    }
#else
    ((void) ciphersuite);
#endif /* POLARSSL_X509_CHECK_KEY_USAGE */

#if defined(POLARSSL_X509_CHECK_EXTENDED_KEY_USAGE)
    if( cert_endpoint == SSL_IS_SERVER )
    {
        ext_oid = OID_SERVER_AUTH;
        ext_len = OID_SIZE( OID_SERVER_AUTH );
    }
    else
    {
        ext_oid = OID_CLIENT_AUTH;
        ext_len = OID_SIZE( OID_CLIENT_AUTH );
    }

    if( x509_crt_check_extended_key_usage( cert, ext_oid, ext_len ) != 0 )
    {
        *flags |= BADCERT_EXT_KEY_USAGE;
        ret = -1;
    }
#endif /* POLARSSL_X509_CHECK_EXTENDED_KEY_USAGE */

    return( ret );
}