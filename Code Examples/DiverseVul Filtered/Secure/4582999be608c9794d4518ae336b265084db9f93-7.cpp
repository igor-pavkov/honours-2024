static int ssl_encrypt_buf( ssl_context *ssl )
{
    size_t i, padlen;

    SSL_DEBUG_MSG( 2, ( "=> encrypt buf" ) );

    /*
     * Add MAC then encrypt
     */
    if( ssl->minor_ver == SSL_MINOR_VERSION_0 )
    {
        if( ssl->transform_out->maclen == 16 )
             ssl_mac_md5( ssl->transform_out->mac_enc,
                          ssl->out_msg, ssl->out_msglen,
                          ssl->out_ctr, ssl->out_msgtype );
        else if( ssl->transform_out->maclen == 20 )
            ssl_mac_sha1( ssl->transform_out->mac_enc,
                          ssl->out_msg, ssl->out_msglen,
                          ssl->out_ctr, ssl->out_msgtype );
        else if( ssl->transform_out->maclen == 32 )
            ssl_mac_sha2( ssl->transform_out->mac_enc,
                          ssl->out_msg, ssl->out_msglen,
                          ssl->out_ctr, ssl->out_msgtype );
        else if( ssl->transform_out->maclen != 0 )
        {
            SSL_DEBUG_MSG( 1, ( "invalid MAC len: %d",
                                ssl->transform_out->maclen ) );
            return( POLARSSL_ERR_SSL_FEATURE_UNAVAILABLE );
        }
    }
    else
    {
        if( ssl->transform_out->maclen == 16 )
        {
            md5_context ctx;
            md5_hmac_starts( &ctx, ssl->transform_out->mac_enc, 16 );
            md5_hmac_update( &ctx, ssl->out_ctr, 13 );
            md5_hmac_update( &ctx, ssl->out_msg, ssl->out_msglen );
            md5_hmac_finish( &ctx, ssl->out_msg + ssl->out_msglen );
            memset( &ctx, 0, sizeof(md5_context));
        }
        else if( ssl->transform_out->maclen == 20 )
        {
            sha1_context ctx;
            sha1_hmac_starts( &ctx, ssl->transform_out->mac_enc, 20 );
            sha1_hmac_update( &ctx, ssl->out_ctr, 13 );
            sha1_hmac_update( &ctx, ssl->out_msg, ssl->out_msglen );
            sha1_hmac_finish( &ctx, ssl->out_msg + ssl->out_msglen );
            memset( &ctx, 0, sizeof(sha1_context));
        }
        else if( ssl->transform_out->maclen == 32 )
        {
            sha2_context ctx;
            sha2_hmac_starts( &ctx, ssl->transform_out->mac_enc, 32, 0 );
            sha2_hmac_update( &ctx, ssl->out_ctr, 13 );
            sha2_hmac_update( &ctx, ssl->out_msg, ssl->out_msglen );
            sha2_hmac_finish( &ctx, ssl->out_msg + ssl->out_msglen );
            memset( &ctx, 0, sizeof(sha2_context));
        }
        else if( ssl->transform_out->maclen != 0 )
        {
            SSL_DEBUG_MSG( 1, ( "invalid MAC len: %d",
                                ssl->transform_out->maclen ) );
            return( POLARSSL_ERR_SSL_FEATURE_UNAVAILABLE );
        }
    }

    SSL_DEBUG_BUF( 4, "computed mac",
                   ssl->out_msg + ssl->out_msglen, ssl->transform_out->maclen );

    ssl->out_msglen += ssl->transform_out->maclen;

    if( ssl->transform_out->ivlen == 0 )
    {
        padlen = 0;

        SSL_DEBUG_MSG( 3, ( "before encrypt: msglen = %d, "
                            "including %d bytes of padding",
                       ssl->out_msglen, 0 ) );

        SSL_DEBUG_BUF( 4, "before encrypt: output payload",
                       ssl->out_msg, ssl->out_msglen );

#if defined(POLARSSL_ARC4_C)
        if( ssl->session_out->ciphersuite == TLS_RSA_WITH_RC4_128_MD5 ||
            ssl->session_out->ciphersuite == TLS_RSA_WITH_RC4_128_SHA )
        {
            arc4_crypt( (arc4_context *) ssl->transform_out->ctx_enc,
                        ssl->out_msglen, ssl->out_msg,
                        ssl->out_msg );
        } else
#endif
#if defined(POLARSSL_CIPHER_NULL_CIPHER)
        if( ssl->session_out->ciphersuite == TLS_RSA_WITH_NULL_MD5 ||
            ssl->session_out->ciphersuite == TLS_RSA_WITH_NULL_SHA ||
            ssl->session_out->ciphersuite == TLS_RSA_WITH_NULL_SHA256 )
        {
        } else
#endif
        return( POLARSSL_ERR_SSL_FEATURE_UNAVAILABLE );
    }
    else if( ssl->transform_out->ivlen == 12 )
    {
        size_t enc_msglen;
        unsigned char *enc_msg;
        unsigned char add_data[13];
        int ret = POLARSSL_ERR_SSL_FEATURE_UNAVAILABLE;

        padlen = 0;
        enc_msglen = ssl->out_msglen;

        memcpy( add_data, ssl->out_ctr, 8 );
        add_data[8]  = ssl->out_msgtype;
        add_data[9]  = ssl->major_ver;
        add_data[10] = ssl->minor_ver;
        add_data[11] = ( ssl->out_msglen >> 8 ) & 0xFF;
        add_data[12] = ssl->out_msglen & 0xFF;

        SSL_DEBUG_BUF( 4, "additional data used for AEAD",
                       add_data, 13 );

#if defined(POLARSSL_AES_C) && defined(POLARSSL_GCM_C)

        if( ssl->session_out->ciphersuite == TLS_RSA_WITH_AES_128_GCM_SHA256 ||
            ssl->session_out->ciphersuite == TLS_RSA_WITH_AES_256_GCM_SHA384 ||
            ssl->session_out->ciphersuite == TLS_DHE_RSA_WITH_AES_128_GCM_SHA256 ||
            ssl->session_out->ciphersuite == TLS_DHE_RSA_WITH_AES_256_GCM_SHA384 )
        {
            /*
             * Generate IV
             */
            ret = ssl->f_rng( ssl->p_rng,
                        ssl->transform_out->iv_enc + ssl->transform_out->fixed_ivlen,
                        ssl->transform_out->ivlen - ssl->transform_out->fixed_ivlen );
            if( ret != 0 )
                return( ret );

            /*
             * Shift message for ivlen bytes and prepend IV
             */
            memmove( ssl->out_msg + ssl->transform_out->ivlen -
                     ssl->transform_out->fixed_ivlen,
                     ssl->out_msg, ssl->out_msglen );
            memcpy( ssl->out_msg,
                    ssl->transform_out->iv_enc + ssl->transform_out->fixed_ivlen,
                    ssl->transform_out->ivlen  - ssl->transform_out->fixed_ivlen );

            /*
             * Fix pointer positions and message length with added IV
             */
            enc_msg = ssl->out_msg + ssl->transform_out->ivlen -
                      ssl->transform_out->fixed_ivlen;
            enc_msglen = ssl->out_msglen;
            ssl->out_msglen += ssl->transform_out->ivlen -
                               ssl->transform_out->fixed_ivlen;

            SSL_DEBUG_MSG( 3, ( "before encrypt: msglen = %d, "
                                "including %d bytes of padding",
                           ssl->out_msglen, 0 ) );

            SSL_DEBUG_BUF( 4, "before encrypt: output payload",
                           ssl->out_msg, ssl->out_msglen );

            /*
             * Adjust for tag
             */
            ssl->out_msglen += 16;
            
            gcm_crypt_and_tag( (gcm_context *) ssl->transform_out->ctx_enc,
                    GCM_ENCRYPT, enc_msglen,
                    ssl->transform_out->iv_enc, ssl->transform_out->ivlen,
                    add_data, 13,
                    enc_msg, enc_msg,
                    16, enc_msg + enc_msglen );

            SSL_DEBUG_BUF( 4, "after encrypt: tag",
                           enc_msg + enc_msglen, 16 );

        } else
#endif
        return( ret );
    }
    else
    {
        unsigned char *enc_msg;
        size_t enc_msglen;

        padlen = ssl->transform_out->ivlen - ( ssl->out_msglen + 1 ) %
                 ssl->transform_out->ivlen;
        if( padlen == ssl->transform_out->ivlen )
            padlen = 0;

        for( i = 0; i <= padlen; i++ )
            ssl->out_msg[ssl->out_msglen + i] = (unsigned char) padlen;

        ssl->out_msglen += padlen + 1;

        enc_msglen = ssl->out_msglen;
        enc_msg = ssl->out_msg;

        /*
         * Prepend per-record IV for block cipher in TLS v1.1 and up as per
         * Method 1 (6.2.3.2. in RFC4346 and RFC5246)
         */
        if( ssl->minor_ver >= SSL_MINOR_VERSION_2 )
        {
            /*
             * Generate IV
             */
            int ret = ssl->f_rng( ssl->p_rng, ssl->transform_out->iv_enc,
                                  ssl->transform_out->ivlen );
            if( ret != 0 )
                return( ret );

            /*
             * Shift message for ivlen bytes and prepend IV
             */
            memmove( ssl->out_msg + ssl->transform_out->ivlen, ssl->out_msg,
                     ssl->out_msglen );
            memcpy( ssl->out_msg, ssl->transform_out->iv_enc,
                    ssl->transform_out->ivlen );

            /*
             * Fix pointer positions and message length with added IV
             */
            enc_msg = ssl->out_msg + ssl->transform_out->ivlen;
            enc_msglen = ssl->out_msglen;
            ssl->out_msglen += ssl->transform_out->ivlen;
        }

        SSL_DEBUG_MSG( 3, ( "before encrypt: msglen = %d, "
                            "including %d bytes of IV and %d bytes of padding",
                       ssl->out_msglen, ssl->transform_out->ivlen, padlen + 1 ) );

        SSL_DEBUG_BUF( 4, "before encrypt: output payload",
                       ssl->out_msg, ssl->out_msglen );

        switch( ssl->transform_out->ivlen )
        {
#if defined(POLARSSL_DES_C)
            case  8:
#if defined(POLARSSL_ENABLE_WEAK_CIPHERSUITES)
                if( ssl->session_out->ciphersuite == TLS_RSA_WITH_DES_CBC_SHA ||
                    ssl->session_out->ciphersuite == TLS_DHE_RSA_WITH_DES_CBC_SHA )
                {
                    des_crypt_cbc( (des_context *) ssl->transform_out->ctx_enc,
                                   DES_ENCRYPT, enc_msglen,
                                   ssl->transform_out->iv_enc, enc_msg, enc_msg );
                }
                else
#endif
                    des3_crypt_cbc( (des3_context *) ssl->transform_out->ctx_enc,
                                    DES_ENCRYPT, enc_msglen,
                                    ssl->transform_out->iv_enc, enc_msg, enc_msg );
                break;
#endif

            case 16:
#if defined(POLARSSL_AES_C)
        if ( ssl->session_out->ciphersuite == TLS_RSA_WITH_AES_128_CBC_SHA ||
             ssl->session_out->ciphersuite == TLS_DHE_RSA_WITH_AES_128_CBC_SHA ||
             ssl->session_out->ciphersuite == TLS_RSA_WITH_AES_256_CBC_SHA ||
             ssl->session_out->ciphersuite == TLS_DHE_RSA_WITH_AES_256_CBC_SHA ||
             ssl->session_out->ciphersuite == TLS_RSA_WITH_AES_128_CBC_SHA256 ||
             ssl->session_out->ciphersuite == TLS_RSA_WITH_AES_256_CBC_SHA256 ||
             ssl->session_out->ciphersuite == TLS_DHE_RSA_WITH_AES_128_CBC_SHA256 ||
             ssl->session_out->ciphersuite == TLS_DHE_RSA_WITH_AES_256_CBC_SHA256 )
        {
                    aes_crypt_cbc( (aes_context *) ssl->transform_out->ctx_enc,
                        AES_ENCRYPT, enc_msglen,
                        ssl->transform_out->iv_enc, enc_msg, enc_msg);
                    break;
        }
#endif

#if defined(POLARSSL_CAMELLIA_C)
        if ( ssl->session_out->ciphersuite == TLS_RSA_WITH_CAMELLIA_128_CBC_SHA ||
             ssl->session_out->ciphersuite == TLS_DHE_RSA_WITH_CAMELLIA_128_CBC_SHA ||
             ssl->session_out->ciphersuite == TLS_RSA_WITH_CAMELLIA_256_CBC_SHA ||
             ssl->session_out->ciphersuite == TLS_DHE_RSA_WITH_CAMELLIA_256_CBC_SHA ||
             ssl->session_out->ciphersuite == TLS_RSA_WITH_CAMELLIA_128_CBC_SHA256 ||
             ssl->session_out->ciphersuite == TLS_DHE_RSA_WITH_CAMELLIA_128_CBC_SHA256 ||
             ssl->session_out->ciphersuite == TLS_RSA_WITH_CAMELLIA_256_CBC_SHA256 ||
             ssl->session_out->ciphersuite == TLS_DHE_RSA_WITH_CAMELLIA_256_CBC_SHA256 )
        {
                    camellia_crypt_cbc( (camellia_context *) ssl->transform_out->ctx_enc,
                        CAMELLIA_ENCRYPT, enc_msglen,
                        ssl->transform_out->iv_enc, enc_msg, enc_msg );
                    break;
        }
#endif

            default:
                return( POLARSSL_ERR_SSL_FEATURE_UNAVAILABLE );
        }
    }

    for( i = 8; i > 0; i-- )
        if( ++ssl->out_ctr[i - 1] != 0 )
            break;

    SSL_DEBUG_MSG( 2, ( "<= encrypt buf" ) );

    return( 0 );
}