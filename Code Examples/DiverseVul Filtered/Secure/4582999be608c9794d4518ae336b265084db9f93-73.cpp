const char *ssl_get_ciphersuite_name( const int ciphersuite_id )
{
    switch( ciphersuite_id )
    {
#if defined(POLARSSL_ARC4_C)
        case TLS_RSA_WITH_RC4_128_MD5:
            return( "TLS-RSA-WITH-RC4-128-MD5" );

        case TLS_RSA_WITH_RC4_128_SHA:
            return( "TLS-RSA-WITH-RC4-128-SHA" );
#endif

#if defined(POLARSSL_DES_C)
        case TLS_RSA_WITH_3DES_EDE_CBC_SHA:
            return( "TLS-RSA-WITH-3DES-EDE-CBC-SHA" );

        case TLS_DHE_RSA_WITH_3DES_EDE_CBC_SHA:
            return( "TLS-DHE-RSA-WITH-3DES-EDE-CBC-SHA" );
#endif

#if defined(POLARSSL_AES_C)
        case TLS_RSA_WITH_AES_128_CBC_SHA:
            return( "TLS-RSA-WITH-AES-128-CBC-SHA" );

        case TLS_DHE_RSA_WITH_AES_128_CBC_SHA:
            return( "TLS-DHE-RSA-WITH-AES-128-CBC-SHA" );

        case TLS_RSA_WITH_AES_256_CBC_SHA:
            return( "TLS-RSA-WITH-AES-256-CBC-SHA" );

        case TLS_DHE_RSA_WITH_AES_256_CBC_SHA:
            return( "TLS-DHE-RSA-WITH-AES-256-CBC-SHA" );

#if defined(POLARSSL_SHA2_C)
        case TLS_RSA_WITH_AES_128_CBC_SHA256:
            return( "TLS-RSA-WITH-AES-128-CBC-SHA256" );

        case TLS_RSA_WITH_AES_256_CBC_SHA256:
            return( "TLS-RSA-WITH-AES-256-CBC-SHA256" );

        case TLS_DHE_RSA_WITH_AES_128_CBC_SHA256:
            return( "TLS-DHE-RSA-WITH-AES-128-CBC-SHA256" );

        case TLS_DHE_RSA_WITH_AES_256_CBC_SHA256:
            return( "TLS-DHE-RSA-WITH-AES-256-CBC-SHA256" );
#endif

#if defined(POLARSSL_GCM_C) && defined(POLARSSL_SHA2_C)
        case TLS_RSA_WITH_AES_128_GCM_SHA256:
            return( "TLS-RSA-WITH-AES-128-GCM-SHA256" );

        case TLS_RSA_WITH_AES_256_GCM_SHA384:
            return( "TLS-RSA-WITH-AES-256-GCM-SHA384" );
#endif

#if defined(POLARSSL_GCM_C) && defined(POLARSSL_SHA4_C)
        case TLS_DHE_RSA_WITH_AES_128_GCM_SHA256:
            return( "TLS-DHE-RSA-WITH-AES-128-GCM-SHA256" );

        case TLS_DHE_RSA_WITH_AES_256_GCM_SHA384:
            return( "TLS-DHE-RSA-WITH-AES-256-GCM-SHA384" );
#endif
#endif /* POLARSSL_AES_C */

#if defined(POLARSSL_CAMELLIA_C)
        case TLS_RSA_WITH_CAMELLIA_128_CBC_SHA:
            return( "TLS-RSA-WITH-CAMELLIA-128-CBC-SHA" );

        case TLS_DHE_RSA_WITH_CAMELLIA_128_CBC_SHA:
            return( "TLS-DHE-RSA-WITH-CAMELLIA-128-CBC-SHA" );

        case TLS_RSA_WITH_CAMELLIA_256_CBC_SHA:
            return( "TLS-RSA-WITH-CAMELLIA-256-CBC-SHA" );

        case TLS_DHE_RSA_WITH_CAMELLIA_256_CBC_SHA:
            return( "TLS-DHE-RSA-WITH-CAMELLIA-256-CBC-SHA" );

#if defined(POLARSSL_SHA2_C)
        case TLS_RSA_WITH_CAMELLIA_128_CBC_SHA256:
            return( "TLS-RSA-WITH-CAMELLIA-128-CBC-SHA256" );

        case TLS_DHE_RSA_WITH_CAMELLIA_128_CBC_SHA256:
            return( "TLS-DHE-RSA-WITH-CAMELLIA-128-CBC-SHA256" );

        case TLS_RSA_WITH_CAMELLIA_256_CBC_SHA256:
            return( "TLS-RSA-WITH-CAMELLIA-256-CBC-SHA256" );

        case TLS_DHE_RSA_WITH_CAMELLIA_256_CBC_SHA256:
            return( "TLS-DHE-RSA-WITH-CAMELLIA-256-CBC-SHA256" );
#endif
#endif

#if defined(POLARSSL_ENABLE_WEAK_CIPHERSUITES)
#if defined(POLARSSL_CIPHER_NULL_CIPHER)
        case TLS_RSA_WITH_NULL_MD5:
            return( "TLS-RSA-WITH-NULL-MD5" );
        case TLS_RSA_WITH_NULL_SHA:
            return( "TLS-RSA-WITH-NULL-SHA" );
        case TLS_RSA_WITH_NULL_SHA256:
            return( "TLS-RSA-WITH-NULL-SHA256" );
#endif /* defined(POLARSSL_CIPHER_NULL_CIPHER) */

#if defined(POLARSSL_DES_C)
        case TLS_RSA_WITH_DES_CBC_SHA:
            return( "TLS-RSA-WITH-DES-CBC-SHA" );
        case TLS_DHE_RSA_WITH_DES_CBC_SHA:
            return( "TLS-DHE-RSA-WITH-DES-CBC-SHA" );
#endif
#endif /* defined(POLARSSL_ENABLE_WEAK_CIPHERSUITES) */

    default:
        break;
    }

    return( "unknown" );
}