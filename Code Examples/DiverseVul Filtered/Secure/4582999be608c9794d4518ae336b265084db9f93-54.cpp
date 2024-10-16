int ssl_derive_keys( ssl_context *ssl )
{
    unsigned char tmp[64];
    unsigned char keyblk[256];
    unsigned char *key1;
    unsigned char *key2;
    unsigned int iv_copy_len;
    ssl_session *session = ssl->session_negotiate;
    ssl_transform *transform = ssl->transform_negotiate;
    ssl_handshake_params *handshake = ssl->handshake;

    SSL_DEBUG_MSG( 2, ( "=> derive keys" ) );

    /*
     * Set appropriate PRF function and other SSL / TLS / TLS1.2 functions
     */
    if( ssl->minor_ver == SSL_MINOR_VERSION_0 )
    {
        handshake->tls_prf = ssl3_prf;
        handshake->calc_verify = ssl_calc_verify_ssl;
        handshake->calc_finished = ssl_calc_finished_ssl;
    }
    else if( ssl->minor_ver < SSL_MINOR_VERSION_3 )
    {
        handshake->tls_prf = tls1_prf;
        handshake->calc_verify = ssl_calc_verify_tls;
        handshake->calc_finished = ssl_calc_finished_tls;
    }
#if defined(POLARSSL_SHA4_C)
    else if( session->ciphersuite == TLS_RSA_WITH_AES_256_GCM_SHA384 ||
             session->ciphersuite == TLS_DHE_RSA_WITH_AES_256_GCM_SHA384 )
    {
        handshake->tls_prf = tls_prf_sha384;
        handshake->calc_verify = ssl_calc_verify_tls_sha384;
        handshake->calc_finished = ssl_calc_finished_tls_sha384;
    }
#endif
    else
    {
        handshake->tls_prf = tls_prf_sha256;
        handshake->calc_verify = ssl_calc_verify_tls_sha256;
        handshake->calc_finished = ssl_calc_finished_tls_sha256;
    }

    /*
     * SSLv3:
     *   master =
     *     MD5( premaster + SHA1( 'A'   + premaster + randbytes ) ) +
     *     MD5( premaster + SHA1( 'BB'  + premaster + randbytes ) ) +
     *     MD5( premaster + SHA1( 'CCC' + premaster + randbytes ) )
     *   
     * TLSv1:
     *   master = PRF( premaster, "master secret", randbytes )[0..47]
     */
    if( handshake->resume == 0 )
    {
        SSL_DEBUG_BUF( 3, "premaster secret", handshake->premaster,
                       handshake->pmslen );

        handshake->tls_prf( handshake->premaster, handshake->pmslen,
                            "master secret",
                            handshake->randbytes, 64, session->master, 48 );

        memset( handshake->premaster, 0, sizeof( handshake->premaster ) );
    }
    else
        SSL_DEBUG_MSG( 3, ( "no premaster (session resumed)" ) );

    /*
     * Swap the client and server random values.
     */
    memcpy( tmp, handshake->randbytes, 64 );
    memcpy( handshake->randbytes, tmp + 32, 32 );
    memcpy( handshake->randbytes + 32, tmp, 32 );
    memset( tmp, 0, sizeof( tmp ) );

    /*
     *  SSLv3:
     *    key block =
     *      MD5( master + SHA1( 'A'    + master + randbytes ) ) +
     *      MD5( master + SHA1( 'BB'   + master + randbytes ) ) +
     *      MD5( master + SHA1( 'CCC'  + master + randbytes ) ) +
     *      MD5( master + SHA1( 'DDDD' + master + randbytes ) ) +
     *      ...
     *
     *  TLSv1:
     *    key block = PRF( master, "key expansion", randbytes )
     */
    handshake->tls_prf( session->master, 48, "key expansion",
                        handshake->randbytes, 64, keyblk, 256 );

    SSL_DEBUG_MSG( 3, ( "ciphersuite = %s",
                   ssl_get_ciphersuite_name( session->ciphersuite ) ) );
    SSL_DEBUG_BUF( 3, "master secret", session->master, 48 );
    SSL_DEBUG_BUF( 4, "random bytes", handshake->randbytes, 64 );
    SSL_DEBUG_BUF( 4, "key block", keyblk, 256 );

    memset( handshake->randbytes, 0, sizeof( handshake->randbytes ) );

    /*
     * Determine the appropriate key, IV and MAC length.
     */
    switch( session->ciphersuite )
    {
#if defined(POLARSSL_ARC4_C)
        case TLS_RSA_WITH_RC4_128_MD5:
            transform->keylen = 16; transform->minlen = 16;
            transform->ivlen  =  0; transform->maclen = 16;
            break;

        case TLS_RSA_WITH_RC4_128_SHA:
            transform->keylen = 16; transform->minlen = 20;
            transform->ivlen  =  0; transform->maclen = 20;
            break;
#endif

#if defined(POLARSSL_DES_C)
        case TLS_RSA_WITH_3DES_EDE_CBC_SHA:
        case TLS_DHE_RSA_WITH_3DES_EDE_CBC_SHA:
            transform->keylen = 24; transform->minlen = 24;
            transform->ivlen  =  8; transform->maclen = 20;
            break;
#endif

#if defined(POLARSSL_AES_C)
        case TLS_RSA_WITH_AES_128_CBC_SHA:
        case TLS_DHE_RSA_WITH_AES_128_CBC_SHA:
            transform->keylen = 16; transform->minlen = 32;
            transform->ivlen  = 16; transform->maclen = 20;
            break;

        case TLS_RSA_WITH_AES_256_CBC_SHA:
        case TLS_DHE_RSA_WITH_AES_256_CBC_SHA:
            transform->keylen = 32; transform->minlen = 32;
            transform->ivlen  = 16; transform->maclen = 20;
            break;

#if defined(POLARSSL_SHA2_C)
        case TLS_RSA_WITH_AES_128_CBC_SHA256:
        case TLS_DHE_RSA_WITH_AES_128_CBC_SHA256:
            transform->keylen = 16; transform->minlen = 32;
            transform->ivlen  = 16; transform->maclen = 32;
            break;

        case TLS_RSA_WITH_AES_256_CBC_SHA256:
        case TLS_DHE_RSA_WITH_AES_256_CBC_SHA256:
            transform->keylen = 32; transform->minlen = 32;
            transform->ivlen  = 16; transform->maclen = 32;
            break;
#endif
#if defined(POLARSSL_GCM_C)
        case TLS_RSA_WITH_AES_128_GCM_SHA256:
        case TLS_DHE_RSA_WITH_AES_128_GCM_SHA256:
            transform->keylen = 16; transform->minlen = 1;
            transform->ivlen  = 12; transform->maclen = 0;
            transform->fixed_ivlen = 4;
            break;

        case TLS_RSA_WITH_AES_256_GCM_SHA384:
        case TLS_DHE_RSA_WITH_AES_256_GCM_SHA384:
            transform->keylen = 32; transform->minlen = 1;
            transform->ivlen  = 12; transform->maclen = 0;
            transform->fixed_ivlen = 4;
            break;
#endif
#endif

#if defined(POLARSSL_CAMELLIA_C)
        case TLS_RSA_WITH_CAMELLIA_128_CBC_SHA:
        case TLS_DHE_RSA_WITH_CAMELLIA_128_CBC_SHA:
            transform->keylen = 16; transform->minlen = 32;
            transform->ivlen  = 16; transform->maclen = 20;
            break;

        case TLS_RSA_WITH_CAMELLIA_256_CBC_SHA:
        case TLS_DHE_RSA_WITH_CAMELLIA_256_CBC_SHA:
            transform->keylen = 32; transform->minlen = 32;
            transform->ivlen  = 16; transform->maclen = 20;
            break;

#if defined(POLARSSL_SHA2_C)
        case TLS_RSA_WITH_CAMELLIA_128_CBC_SHA256:
        case TLS_DHE_RSA_WITH_CAMELLIA_128_CBC_SHA256:
            transform->keylen = 16; transform->minlen = 32;
            transform->ivlen  = 16; transform->maclen = 32;
            break;

        case TLS_RSA_WITH_CAMELLIA_256_CBC_SHA256:
        case TLS_DHE_RSA_WITH_CAMELLIA_256_CBC_SHA256:
            transform->keylen = 32; transform->minlen = 32;
            transform->ivlen  = 16; transform->maclen = 32;
            break;
#endif
#endif

#if defined(POLARSSL_ENABLE_WEAK_CIPHERSUITES)
#if defined(POLARSSL_CIPHER_NULL_CIPHER)
        case TLS_RSA_WITH_NULL_MD5:
            transform->keylen = 0; transform->minlen = 0;
            transform->ivlen  = 0; transform->maclen = 16;
            break;

        case TLS_RSA_WITH_NULL_SHA:
            transform->keylen = 0; transform->minlen = 0;
            transform->ivlen  = 0; transform->maclen = 20;
            break;

        case TLS_RSA_WITH_NULL_SHA256:
            transform->keylen = 0; transform->minlen = 0;
            transform->ivlen  = 0; transform->maclen = 32;
            break;
#endif /* defined(POLARSSL_CIPHER_NULL_CIPHER) */

#if defined(POLARSSL_DES_C)
        case TLS_RSA_WITH_DES_CBC_SHA:
        case TLS_DHE_RSA_WITH_DES_CBC_SHA:
            transform->keylen =  8; transform->minlen = 8;
            transform->ivlen  =  8; transform->maclen = 20;
            break;
#endif
#endif /* defined(POLARSSL_ENABLE_WEAK_CIPHERSUITES) */

        default:
            SSL_DEBUG_MSG( 1, ( "ciphersuite %s is not available",
                           ssl_get_ciphersuite_name( session->ciphersuite ) ) );
            return( POLARSSL_ERR_SSL_FEATURE_UNAVAILABLE );
    }

    SSL_DEBUG_MSG( 3, ( "keylen: %d, minlen: %d, ivlen: %d, maclen: %d",
                   transform->keylen, transform->minlen, transform->ivlen,
                   transform->maclen ) );

    /*
     * Finally setup the cipher contexts, IVs and MAC secrets.
     */
    if( ssl->endpoint == SSL_IS_CLIENT )
    {
        key1 = keyblk + transform->maclen * 2;
        key2 = keyblk + transform->maclen * 2 + transform->keylen;

        memcpy( transform->mac_enc, keyblk,  transform->maclen );
        memcpy( transform->mac_dec, keyblk + transform->maclen,
                transform->maclen );

        /*
         * This is not used in TLS v1.1.
         */
        iv_copy_len = ( transform->fixed_ivlen ) ?
                            transform->fixed_ivlen : transform->ivlen;
        memcpy( transform->iv_enc, key2 + transform->keylen,  iv_copy_len );
        memcpy( transform->iv_dec, key2 + transform->keylen + iv_copy_len,
                iv_copy_len );
    }
    else
    {
        key1 = keyblk + transform->maclen * 2 + transform->keylen;
        key2 = keyblk + transform->maclen * 2;

        memcpy( transform->mac_dec, keyblk,  transform->maclen );
        memcpy( transform->mac_enc, keyblk + transform->maclen,
                transform->maclen );

        /*
         * This is not used in TLS v1.1.
         */
        iv_copy_len = ( transform->fixed_ivlen ) ?
                            transform->fixed_ivlen : transform->ivlen;
        memcpy( transform->iv_dec, key1 + transform->keylen,  iv_copy_len );
        memcpy( transform->iv_enc, key1 + transform->keylen + iv_copy_len,
                iv_copy_len );
    }

#if defined(POLARSSL_SSL_HW_RECORD_ACCEL)
    if( ssl_hw_record_init != NULL)
    {
        int ret = 0;

        SSL_DEBUG_MSG( 2, ( "going for ssl_hw_record_init()" ) );

        if( ( ret = ssl_hw_record_init( ssl, key1, key2, transform->iv_enc,
                                        transform->iv_dec, transform->mac_enc,
                                        transform->mac_dec ) ) != 0 )
        {
            SSL_DEBUG_RET( 1, "ssl_hw_record_init", ret );
            return POLARSSL_ERR_SSL_HW_ACCEL_FAILED;
        }
    }
#endif

    switch( session->ciphersuite )
    {
#if defined(POLARSSL_ARC4_C)
        case TLS_RSA_WITH_RC4_128_MD5:
        case TLS_RSA_WITH_RC4_128_SHA:
            arc4_setup( (arc4_context *) transform->ctx_enc, key1,
                        transform->keylen );
            arc4_setup( (arc4_context *) transform->ctx_dec, key2,
                        transform->keylen );
            break;
#endif

#if defined(POLARSSL_DES_C)
        case TLS_RSA_WITH_3DES_EDE_CBC_SHA:
        case TLS_DHE_RSA_WITH_3DES_EDE_CBC_SHA:
            des3_set3key_enc( (des3_context *) transform->ctx_enc, key1 );
            des3_set3key_dec( (des3_context *) transform->ctx_dec, key2 );
            break;
#endif

#if defined(POLARSSL_AES_C)
        case TLS_RSA_WITH_AES_128_CBC_SHA:
        case TLS_DHE_RSA_WITH_AES_128_CBC_SHA:
        case TLS_RSA_WITH_AES_128_CBC_SHA256:
        case TLS_DHE_RSA_WITH_AES_128_CBC_SHA256:
            aes_setkey_enc( (aes_context *) transform->ctx_enc, key1, 128 );
            aes_setkey_dec( (aes_context *) transform->ctx_dec, key2, 128 );
            break;

        case TLS_RSA_WITH_AES_256_CBC_SHA:
        case TLS_DHE_RSA_WITH_AES_256_CBC_SHA:
        case TLS_RSA_WITH_AES_256_CBC_SHA256:
        case TLS_DHE_RSA_WITH_AES_256_CBC_SHA256:
            aes_setkey_enc( (aes_context *) transform->ctx_enc, key1, 256 );
            aes_setkey_dec( (aes_context *) transform->ctx_dec, key2, 256 );
            break;

#if defined(POLARSSL_GCM_C)
        case TLS_RSA_WITH_AES_128_GCM_SHA256:
        case TLS_DHE_RSA_WITH_AES_128_GCM_SHA256:
            gcm_init( (gcm_context *) transform->ctx_enc, key1, 128 );
            gcm_init( (gcm_context *) transform->ctx_dec, key2, 128 );
            break;

        case TLS_RSA_WITH_AES_256_GCM_SHA384:
        case TLS_DHE_RSA_WITH_AES_256_GCM_SHA384:
            gcm_init( (gcm_context *) transform->ctx_enc, key1, 256 );
            gcm_init( (gcm_context *) transform->ctx_dec, key2, 256 );
            break;
#endif
#endif

#if defined(POLARSSL_CAMELLIA_C)
        case TLS_RSA_WITH_CAMELLIA_128_CBC_SHA:
        case TLS_DHE_RSA_WITH_CAMELLIA_128_CBC_SHA:
        case TLS_RSA_WITH_CAMELLIA_128_CBC_SHA256:
        case TLS_DHE_RSA_WITH_CAMELLIA_128_CBC_SHA256:
            camellia_setkey_enc( (camellia_context *) transform->ctx_enc, key1, 128 );
            camellia_setkey_dec( (camellia_context *) transform->ctx_dec, key2, 128 );
            break;

        case TLS_RSA_WITH_CAMELLIA_256_CBC_SHA:
        case TLS_DHE_RSA_WITH_CAMELLIA_256_CBC_SHA:
        case TLS_RSA_WITH_CAMELLIA_256_CBC_SHA256:
        case TLS_DHE_RSA_WITH_CAMELLIA_256_CBC_SHA256:
            camellia_setkey_enc( (camellia_context *) transform->ctx_enc, key1, 256 );
            camellia_setkey_dec( (camellia_context *) transform->ctx_dec, key2, 256 );
            break;
#endif

#if defined(POLARSSL_ENABLE_WEAK_CIPHERSUITES)
#if defined(POLARSSL_CIPHER_NULL_CIPHER)
        case TLS_RSA_WITH_NULL_MD5:
        case TLS_RSA_WITH_NULL_SHA:
        case TLS_RSA_WITH_NULL_SHA256:
            break;
#endif /* defined(POLARSSL_CIPHER_NULL_CIPHER) */

#if defined(POLARSSL_DES_C)
        case TLS_RSA_WITH_DES_CBC_SHA:
        case TLS_DHE_RSA_WITH_DES_CBC_SHA:
            des_setkey_enc( (des_context *) transform->ctx_enc, key1 );
            des_setkey_dec( (des_context *) transform->ctx_dec, key2 );
            break;
#endif
#endif /* defined(POLARSSL_ENABLE_WEAK_CIPHERSUITES) */

        default:
            return( POLARSSL_ERR_SSL_FEATURE_UNAVAILABLE );
    }

    memset( keyblk, 0, sizeof( keyblk ) );

#if defined(POLARSSL_ZLIB_SUPPORT)
    // Initialize compression
    //
    if( session->compression == SSL_COMPRESS_DEFLATE )
    {
        SSL_DEBUG_MSG( 3, ( "Initializing zlib states" ) );

        memset( &transform->ctx_deflate, 0, sizeof( transform->ctx_deflate ) );
        memset( &transform->ctx_inflate, 0, sizeof( transform->ctx_inflate ) );

        if( deflateInit( &transform->ctx_deflate, Z_DEFAULT_COMPRESSION ) != Z_OK ||
            inflateInit( &transform->ctx_inflate ) != Z_OK )
        {
            SSL_DEBUG_MSG( 1, ( "Failed to initialize compression" ) );
            return( POLARSSL_ERR_SSL_COMPRESSION_FAILED );
        }
    }
#endif /* POLARSSL_ZLIB_SUPPORT */

    SSL_DEBUG_MSG( 2, ( "<= derive keys" ) );

    return( 0 );
}