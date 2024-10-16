static int ssl_encrypt_buf( ssl_context *ssl )
{
    size_t i;
    cipher_mode_t mode;
    int auth_done = 0;

    SSL_DEBUG_MSG( 2, ( "=> encrypt buf" ) );

    if( ssl->session_out == NULL || ssl->transform_out == NULL )
    {
        SSL_DEBUG_MSG( 1, ( "should never happen" ) );
        return( POLARSSL_ERR_SSL_INTERNAL_ERROR );
    }

    mode = cipher_get_cipher_mode( &ssl->transform_out->cipher_ctx_enc );

    SSL_DEBUG_BUF( 4, "before encrypt: output payload",
                      ssl->out_msg, ssl->out_msglen );

    /*
     * Add MAC before if needed
     */
#if defined(POLARSSL_SOME_MODES_USE_MAC)
    if( mode == POLARSSL_MODE_STREAM ||
        ( mode == POLARSSL_MODE_CBC
#if defined(POLARSSL_SSL_ENCRYPT_THEN_MAC)
          && ssl->session_out->encrypt_then_mac == SSL_ETM_DISABLED
#endif
        ) )
    {
#if defined(POLARSSL_SSL_PROTO_SSL3)
        if( ssl->minor_ver == SSL_MINOR_VERSION_0 )
        {
            ssl_mac( &ssl->transform_out->md_ctx_enc,
                      ssl->transform_out->mac_enc,
                      ssl->out_msg, ssl->out_msglen,
                      ssl->out_ctr, ssl->out_msgtype );
        }
        else
#endif
#if defined(POLARSSL_SSL_PROTO_TLS1) || defined(POLARSSL_SSL_PROTO_TLS1_1) || \
        defined(POLARSSL_SSL_PROTO_TLS1_2)
        if( ssl->minor_ver >= SSL_MINOR_VERSION_1 )
        {
            md_hmac_update( &ssl->transform_out->md_ctx_enc, ssl->out_ctr, 13 );
            md_hmac_update( &ssl->transform_out->md_ctx_enc,
                             ssl->out_msg, ssl->out_msglen );
            md_hmac_finish( &ssl->transform_out->md_ctx_enc,
                             ssl->out_msg + ssl->out_msglen );
            md_hmac_reset( &ssl->transform_out->md_ctx_enc );
        }
        else
#endif
        {
            SSL_DEBUG_MSG( 1, ( "should never happen" ) );
            return( POLARSSL_ERR_SSL_INTERNAL_ERROR );
        }

        SSL_DEBUG_BUF( 4, "computed mac",
                       ssl->out_msg + ssl->out_msglen,
                       ssl->transform_out->maclen );

        ssl->out_msglen += ssl->transform_out->maclen;
        auth_done++;
    }
#endif /* AEAD not the only option */

    /*
     * Encrypt
     */
#if defined(POLARSSL_ARC4_C) || defined(POLARSSL_CIPHER_NULL_CIPHER)
    if( mode == POLARSSL_MODE_STREAM )
    {
        int ret;
        size_t olen = 0;

        SSL_DEBUG_MSG( 3, ( "before encrypt: msglen = %d, "
                            "including %d bytes of padding",
                       ssl->out_msglen, 0 ) );

        if( ( ret = cipher_crypt( &ssl->transform_out->cipher_ctx_enc,
                                   ssl->transform_out->iv_enc,
                                   ssl->transform_out->ivlen,
                                   ssl->out_msg, ssl->out_msglen,
                                   ssl->out_msg, &olen ) ) != 0 )
        {
            SSL_DEBUG_RET( 1, "cipher_crypt", ret );
            return( ret );
        }

        if( ssl->out_msglen != olen )
        {
            SSL_DEBUG_MSG( 1, ( "should never happen" ) );
            return( POLARSSL_ERR_SSL_INTERNAL_ERROR );
        }
    }
    else
#endif /* POLARSSL_ARC4_C || POLARSSL_CIPHER_NULL_CIPHER */
#if defined(POLARSSL_GCM_C) || defined(POLARSSL_CCM_C)
    if( mode == POLARSSL_MODE_GCM ||
        mode == POLARSSL_MODE_CCM )
    {
        int ret;
        size_t enc_msglen, olen;
        unsigned char *enc_msg;
        unsigned char add_data[13];
        unsigned char taglen = ssl->transform_out->ciphersuite_info->flags &
                               POLARSSL_CIPHERSUITE_SHORT_TAG ? 8 : 16;

        memcpy( add_data, ssl->out_ctr, 8 );
        add_data[8]  = ssl->out_msgtype;
        add_data[9]  = ssl->major_ver;
        add_data[10] = ssl->minor_ver;
        add_data[11] = ( ssl->out_msglen >> 8 ) & 0xFF;
        add_data[12] = ssl->out_msglen & 0xFF;

        SSL_DEBUG_BUF( 4, "additional data used for AEAD",
                       add_data, 13 );

        /*
         * Generate IV
         */
#if defined(POLARSSL_SSL_AEAD_RANDOM_IV)
        ret = ssl->f_rng( ssl->p_rng,
                ssl->transform_out->iv_enc + ssl->transform_out->fixed_ivlen,
                ssl->transform_out->ivlen - ssl->transform_out->fixed_ivlen );
        if( ret != 0 )
            return( ret );

        memcpy( ssl->out_iv,
                ssl->transform_out->iv_enc + ssl->transform_out->fixed_ivlen,
                ssl->transform_out->ivlen - ssl->transform_out->fixed_ivlen );
#else
        if( ssl->transform_out->ivlen - ssl->transform_out->fixed_ivlen != 8 )
        {
            /* Reminder if we ever add an AEAD mode with a different size */
            SSL_DEBUG_MSG( 1, ( "should never happen" ) );
            return( POLARSSL_ERR_SSL_INTERNAL_ERROR );
        }

        memcpy( ssl->transform_out->iv_enc + ssl->transform_out->fixed_ivlen,
                             ssl->out_ctr, 8 );
        memcpy( ssl->out_iv, ssl->out_ctr, 8 );
#endif

        SSL_DEBUG_BUF( 4, "IV used", ssl->out_iv,
                ssl->transform_out->ivlen - ssl->transform_out->fixed_ivlen );

        /*
         * Fix pointer positions and message length with added IV
         */
        enc_msg = ssl->out_msg;
        enc_msglen = ssl->out_msglen;
        ssl->out_msglen += ssl->transform_out->ivlen -
                           ssl->transform_out->fixed_ivlen;

        SSL_DEBUG_MSG( 3, ( "before encrypt: msglen = %d, "
                            "including %d bytes of padding",
                       ssl->out_msglen, 0 ) );

        /*
         * Encrypt and authenticate
         */
        if( ( ret = cipher_auth_encrypt( &ssl->transform_out->cipher_ctx_enc,
                                         ssl->transform_out->iv_enc,
                                         ssl->transform_out->ivlen,
                                         add_data, 13,
                                         enc_msg, enc_msglen,
                                         enc_msg, &olen,
                                         enc_msg + enc_msglen, taglen ) ) != 0 )
        {
            SSL_DEBUG_RET( 1, "cipher_auth_encrypt", ret );
            return( ret );
        }

        if( olen != enc_msglen )
        {
            SSL_DEBUG_MSG( 1, ( "should never happen" ) );
            return( POLARSSL_ERR_SSL_INTERNAL_ERROR );
        }

        ssl->out_msglen += taglen;
        auth_done++;

        SSL_DEBUG_BUF( 4, "after encrypt: tag", enc_msg + enc_msglen, taglen );
    }
    else
#endif /* POLARSSL_GCM_C || POLARSSL_CCM_C */
#if defined(POLARSSL_CIPHER_MODE_CBC) &&                                    \
    ( defined(POLARSSL_AES_C) || defined(POLARSSL_CAMELLIA_C) )
    if( mode == POLARSSL_MODE_CBC )
    {
        int ret;
        unsigned char *enc_msg;
        size_t enc_msglen, padlen, olen = 0;

        padlen = ssl->transform_out->ivlen - ( ssl->out_msglen + 1 ) %
                 ssl->transform_out->ivlen;
        if( padlen == ssl->transform_out->ivlen )
            padlen = 0;

        for( i = 0; i <= padlen; i++ )
            ssl->out_msg[ssl->out_msglen + i] = (unsigned char) padlen;

        ssl->out_msglen += padlen + 1;

        enc_msglen = ssl->out_msglen;
        enc_msg = ssl->out_msg;

#if defined(POLARSSL_SSL_PROTO_TLS1_1) || defined(POLARSSL_SSL_PROTO_TLS1_2)
        /*
         * Prepend per-record IV for block cipher in TLS v1.1 and up as per
         * Method 1 (6.2.3.2. in RFC4346 and RFC5246)
         */
        if( ssl->minor_ver >= SSL_MINOR_VERSION_2 )
        {
            /*
             * Generate IV
             */
            ret = ssl->f_rng( ssl->p_rng, ssl->transform_out->iv_enc,
                                  ssl->transform_out->ivlen );
            if( ret != 0 )
                return( ret );

            memcpy( ssl->out_iv, ssl->transform_out->iv_enc,
                    ssl->transform_out->ivlen );

            /*
             * Fix pointer positions and message length with added IV
             */
            enc_msg = ssl->out_msg;
            enc_msglen = ssl->out_msglen;
            ssl->out_msglen += ssl->transform_out->ivlen;
        }
#endif /* POLARSSL_SSL_PROTO_TLS1_1 || POLARSSL_SSL_PROTO_TLS1_2 */

        SSL_DEBUG_MSG( 3, ( "before encrypt: msglen = %d, "
                            "including %d bytes of IV and %d bytes of padding",
                            ssl->out_msglen, ssl->transform_out->ivlen,
                            padlen + 1 ) );

        if( ( ret = cipher_crypt( &ssl->transform_out->cipher_ctx_enc,
                                   ssl->transform_out->iv_enc,
                                   ssl->transform_out->ivlen,
                                   enc_msg, enc_msglen,
                                   enc_msg, &olen ) ) != 0 )
        {
            SSL_DEBUG_RET( 1, "cipher_crypt", ret );
            return( ret );
        }

        if( enc_msglen != olen )
        {
            SSL_DEBUG_MSG( 1, ( "should never happen" ) );
            return( POLARSSL_ERR_SSL_INTERNAL_ERROR );
        }

#if defined(POLARSSL_SSL_PROTO_SSL3) || defined(POLARSSL_SSL_PROTO_TLS1)
        if( ssl->minor_ver < SSL_MINOR_VERSION_2 )
        {
            /*
             * Save IV in SSL3 and TLS1
             */
            memcpy( ssl->transform_out->iv_enc,
                    ssl->transform_out->cipher_ctx_enc.iv,
                    ssl->transform_out->ivlen );
        }
#endif

#if defined(POLARSSL_SSL_ENCRYPT_THEN_MAC)
        if( auth_done == 0 )
        {
            /*
             * MAC(MAC_write_key, seq_num +
             *     TLSCipherText.type +
             *     TLSCipherText.version +
             *     length_of( (IV +) ENC(...) ) +
             *     IV + // except for TLS 1.0
             *     ENC(content + padding + padding_length));
             */
            unsigned char pseudo_hdr[13];

            SSL_DEBUG_MSG( 3, ( "using encrypt then mac" ) );

            memcpy( pseudo_hdr +  0, ssl->out_ctr, 8 );
            memcpy( pseudo_hdr +  8, ssl->out_hdr, 3 );
            pseudo_hdr[11] = (unsigned char)( ( ssl->out_msglen >> 8 ) & 0xFF );
            pseudo_hdr[12] = (unsigned char)( ( ssl->out_msglen      ) & 0xFF );

            SSL_DEBUG_BUF( 4, "MAC'd meta-data", pseudo_hdr, 13 );

            md_hmac_update( &ssl->transform_out->md_ctx_enc, pseudo_hdr, 13 );
            md_hmac_update( &ssl->transform_out->md_ctx_enc,
                             ssl->out_iv, ssl->out_msglen );
            md_hmac_finish( &ssl->transform_out->md_ctx_enc,
                             ssl->out_iv + ssl->out_msglen );
            md_hmac_reset( &ssl->transform_out->md_ctx_enc );

            ssl->out_msglen += ssl->transform_out->maclen;
            auth_done++;
        }
#endif /* POLARSSL_SSL_ENCRYPT_THEN_MAC */
    }
    else
#endif /* POLARSSL_CIPHER_MODE_CBC &&
          ( POLARSSL_AES_C || POLARSSL_CAMELLIA_C ) */
    {
        SSL_DEBUG_MSG( 1, ( "should never happen" ) );
        return( POLARSSL_ERR_SSL_INTERNAL_ERROR );
    }

    /* Make extra sure authentication was performed, exactly once */
    if( auth_done != 1 )
    {
        SSL_DEBUG_MSG( 1, ( "should never happen" ) );
        return( POLARSSL_ERR_SSL_INTERNAL_ERROR );
    }

    for( i = 8; i > 0; i-- )
        if( ++ssl->out_ctr[i - 1] != 0 )
            break;

    /* The loops goes to its end iff the counter is wrapping */
    if( i == 0 )
    {
        SSL_DEBUG_MSG( 1, ( "outgoing message counter would wrap" ) );
        return( POLARSSL_ERR_SSL_COUNTER_WRAPPING );
    }

    SSL_DEBUG_MSG( 2, ( "<= encrypt buf" ) );

    return( 0 );
}