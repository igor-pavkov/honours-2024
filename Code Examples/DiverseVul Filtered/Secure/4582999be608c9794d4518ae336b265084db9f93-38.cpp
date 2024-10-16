int ssl_get_ciphersuite_id( const char *ciphersuite_name )
{
#if defined(POLARSSL_ARC4_C)
    if (0 == strcasecmp(ciphersuite_name, "TLS-RSA-WITH-RC4-128-MD5"))
        return( TLS_RSA_WITH_RC4_128_MD5 );
    if (0 == strcasecmp(ciphersuite_name, "TLS-RSA-WITH-RC4-128-SHA"))
        return( TLS_RSA_WITH_RC4_128_SHA );
#endif

#if defined(POLARSSL_DES_C)
    if (0 == strcasecmp(ciphersuite_name, "TLS-RSA-WITH-3DES-EDE-CBC-SHA"))
        return( TLS_RSA_WITH_3DES_EDE_CBC_SHA );
    if (0 == strcasecmp(ciphersuite_name, "TLS-DHE-RSA-WITH-3DES-EDE-CBC-SHA"))
        return( TLS_DHE_RSA_WITH_3DES_EDE_CBC_SHA );
#endif

#if defined(POLARSSL_AES_C)
    if (0 == strcasecmp(ciphersuite_name, "TLS-RSA-WITH-AES-128-CBC-SHA"))
        return( TLS_RSA_WITH_AES_128_CBC_SHA );
    if (0 == strcasecmp(ciphersuite_name, "TLS-DHE-RSA-WITH-AES-128-CBC-SHA"))
        return( TLS_DHE_RSA_WITH_AES_128_CBC_SHA );
    if (0 == strcasecmp(ciphersuite_name, "TLS-RSA-WITH-AES-256-CBC-SHA"))
        return( TLS_RSA_WITH_AES_256_CBC_SHA );
    if (0 == strcasecmp(ciphersuite_name, "TLS-DHE-RSA-WITH-AES-256-CBC-SHA"))
        return( TLS_DHE_RSA_WITH_AES_256_CBC_SHA );

#if defined(POLARSSL_SHA2_C)
    if (0 == strcasecmp(ciphersuite_name, "TLS-RSA-WITH-AES-128-CBC-SHA256"))
        return( TLS_RSA_WITH_AES_128_CBC_SHA256 );
    if (0 == strcasecmp(ciphersuite_name, "TLS-RSA-WITH-AES-256-CBC-SHA256"))
        return( TLS_RSA_WITH_AES_256_CBC_SHA256 );
    if (0 == strcasecmp(ciphersuite_name, "TLS-DHE-RSA-WITH-AES-128-CBC-SHA256"))
        return( TLS_DHE_RSA_WITH_AES_128_CBC_SHA256 );
    if (0 == strcasecmp(ciphersuite_name, "TLS-DHE-RSA-WITH-AES-256-CBC-SHA256"))
        return( TLS_DHE_RSA_WITH_AES_256_CBC_SHA256 );
#endif

#if defined(POLARSSL_GCM_C) && defined(POLARSSL_SHA2_C)
    if (0 == strcasecmp(ciphersuite_name, "TLS-RSA-WITH-AES-128-GCM-SHA256"))
        return( TLS_RSA_WITH_AES_128_GCM_SHA256 );
    if (0 == strcasecmp(ciphersuite_name, "TLS-RSA-WITH-AES-256-GCM-SHA384"))
        return( TLS_RSA_WITH_AES_256_GCM_SHA384 );
#endif

#if defined(POLARSSL_GCM_C) && defined(POLARSSL_SHA2_C)
    if (0 == strcasecmp(ciphersuite_name, "TLS-DHE-RSA-WITH-AES-128-GCM-SHA256"))
        return( TLS_DHE_RSA_WITH_AES_128_GCM_SHA256 );
    if (0 == strcasecmp(ciphersuite_name, "TLS-DHE-RSA-WITH-AES-256-GCM-SHA384"))
        return( TLS_DHE_RSA_WITH_AES_256_GCM_SHA384 );
#endif
#endif

#if defined(POLARSSL_CAMELLIA_C)
    if (0 == strcasecmp(ciphersuite_name, "TLS-RSA-WITH-CAMELLIA-128-CBC-SHA"))
        return( TLS_RSA_WITH_CAMELLIA_128_CBC_SHA );
    if (0 == strcasecmp(ciphersuite_name, "TLS-DHE-RSA-WITH-CAMELLIA-128-CBC-SHA"))
        return( TLS_DHE_RSA_WITH_CAMELLIA_128_CBC_SHA );
    if (0 == strcasecmp(ciphersuite_name, "TLS-RSA-WITH-CAMELLIA-256-CBC-SHA"))
        return( TLS_RSA_WITH_CAMELLIA_256_CBC_SHA );
    if (0 == strcasecmp(ciphersuite_name, "TLS-DHE-RSA-WITH-CAMELLIA-256-CBC-SHA"))
        return( TLS_DHE_RSA_WITH_CAMELLIA_256_CBC_SHA );

#if defined(POLARSSL_SHA2_C)
    if (0 == strcasecmp(ciphersuite_name, "TLS-RSA-WITH-CAMELLIA-128-CBC-SHA256"))
        return( TLS_RSA_WITH_CAMELLIA_128_CBC_SHA256 );
    if (0 == strcasecmp(ciphersuite_name, "TLS-DHE-RSA-WITH-CAMELLIA-128-CBC-SHA256"))
        return( TLS_DHE_RSA_WITH_CAMELLIA_128_CBC_SHA256 );
    if (0 == strcasecmp(ciphersuite_name, "TLS-RSA-WITH-CAMELLIA-256-CBC-SHA256"))
        return( TLS_RSA_WITH_CAMELLIA_256_CBC_SHA256 );
    if (0 == strcasecmp(ciphersuite_name, "TLS-DHE-RSA-WITH-CAMELLIA-256-CBC-SHA256"))
        return( TLS_DHE_RSA_WITH_CAMELLIA_256_CBC_SHA256 );
#endif
#endif

#if defined(POLARSSL_ENABLE_WEAK_CIPHERSUITES)
#if defined(POLARSSL_CIPHER_NULL_CIPHER)
    if (0 == strcasecmp(ciphersuite_name, "TLS-RSA-WITH-NULL-MD5"))
        return( TLS_RSA_WITH_NULL_MD5 );
    if (0 == strcasecmp(ciphersuite_name, "TLS-RSA-WITH-NULL-SHA"))
        return( TLS_RSA_WITH_NULL_SHA );
    if (0 == strcasecmp(ciphersuite_name, "TLS-RSA-WITH-NULL-SHA256"))
        return( TLS_RSA_WITH_NULL_SHA256 );
#endif /* defined(POLARSSL_CIPHER_NULL_CIPHER) */

#if defined(POLARSSL_DES_C)
    if (0 == strcasecmp(ciphersuite_name, "TLS-RSA-WITH-DES-CBC-SHA"))
        return( TLS_RSA_WITH_DES_CBC_SHA );
    if (0 == strcasecmp(ciphersuite_name, "TLS-DHE-RSA-WITH-DES-CBC-SHA"))
        return( TLS_DHE_RSA_WITH_DES_CBC_SHA );
#endif
#endif /* defined(POLARSSL_ENABLE_WEAK_CIPHERSUITES) */

    return( 0 );
}