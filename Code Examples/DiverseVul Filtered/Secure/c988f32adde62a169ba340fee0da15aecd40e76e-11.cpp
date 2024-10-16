static int ssl_decrypt_buf( ssl_context *ssl )
{
    size_t i;
    cipher_mode_t mode;
    int auth_done = 0;
#if defined(POLARSSL_SOME_MODES_USE_MAC)
    size_t padlen = 0, correct = 1;
#endif

    SSL_DEBUG_MSG( 2, ( "=> decrypt buf" ) );

    if( ssl->session_in == NULL || ssl->transform_in == NULL )
    {
        SSL_DEBUG_MSG( 1, ( "should never happen" ) );
        return( POLARSSL_ERR_SSL_INTERNAL_ERROR );
    }

    mode = cipher_get_cipher_mode( &ssl->transform_in->cipher_ctx_dec );

    if( ssl->in_msglen < ssl->transform_in->minlen )
    {
        SSL_DEBUG_MSG( 1, ( "in_msglen (%d) < minlen (%d)",
                       ssl->in_msglen, ssl->transform_in->minlen ) );
        return( POLARSSL_ERR_SSL_INVALID_MAC );
    }

#if defined(POLARSSL_ARC4_C) || defined(POLARSSL_CIPHER_NULL_CIPHER)
    if( mode == POLARSSL_MODE_STREAM )
    {
        int ret;
        size_t olen = 0;

        padlen = 0;

        if( ( ret = cipher_crypt( &ssl->transform_in->cipher_ctx_dec,
                                   ssl->transform_in->iv_dec,
                                   ssl->transform_in->ivlen,
                                   ssl->in_msg, ssl->in_msglen,
                                   ssl->in_msg, &olen ) ) != 0 )
        {
            SSL_DEBUG_RET( 1, "cipher_crypt", ret );
            return( ret );
        }

        if( ssl->in_msglen != olen )
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
        size_t dec_msglen, olen;
        unsigned char *dec_msg;
        unsigned char *dec_msg_result;
        unsigned char add_data[13];
        unsigned char taglen = ssl->transform_in->ciphersuite_info->flags &
                               POLARSSL_CIPHERSUITE_SHORT_TAG ? 8 : 16;
        unsigned char explicit_iv_len =  ssl->transform_in->ivlen -
                                         ssl->transform_in->fixed_ivlen;

        if( ssl->in_msglen < (size_t) explicit_iv_len + taglen )
        {
            SSL_DEBUG_MSG( 1, ( "msglen (%d) < explicit_iv_len (%d) "
                                "+ taglen (%d)", ssl->in_msglen,
                                explicit_iv_len, taglen ) );
            return( POLARSSL_ERR_SSL_INVALID_MAC );
        }
        dec_msglen = ssl->in_msglen - explicit_iv_len - taglen;

        dec_msg = ssl->in_msg;
        dec_msg_result = ssl->in_msg;
        ssl->in_msglen = dec_msglen;

        memcpy( add_data, ssl->in_ctr, 8 );
        add_data[8]  = ssl->in_msgtype;
        add_data[9]  = ssl->major_ver;
        add_data[10] = ssl->minor_ver;
        add_data[11] = ( ssl->in_msglen >> 8 ) & 0xFF;
        add_data[12] = ssl->in_msglen & 0xFF;

        SSL_DEBUG_BUF( 4, "additional data used for AEAD",
                       add_data, 13 );

        memcpy( ssl->transform_in->iv_dec + ssl->transform_in->fixed_ivlen,
                ssl->in_iv,
                ssl->transform_in->ivlen - ssl->transform_in->fixed_ivlen );

        SSL_DEBUG_BUF( 4, "IV used", ssl->transform_in->iv_dec,
                                     ssl->transform_in->ivlen );
        SSL_DEBUG_BUF( 4, "TAG used", dec_msg + dec_msglen, taglen );

        /*
         * Decrypt and authenticate
         */
        if( ( ret = cipher_auth_decrypt( &ssl->transform_in->cipher_ctx_dec,
                                         ssl->transform_in->iv_dec,
                                         ssl->transform_in->ivlen,
                                         add_data, 13,
                                         dec_msg, dec_msglen,
                                         dec_msg_result, &olen,
                                         dec_msg + dec_msglen, taglen ) ) != 0 )
        {
            SSL_DEBUG_RET( 1, "cipher_auth_decrypt", ret );

            if( ret == POLARSSL_ERR_CIPHER_AUTH_FAILED )
                return( POLARSSL_ERR_SSL_INVALID_MAC );

            return( ret );
        }
        auth_done++;

        if( olen != dec_msglen )
        {
            SSL_DEBUG_MSG( 1, ( "should never happen" ) );
            return( POLARSSL_ERR_SSL_INTERNAL_ERROR );
        }
    }
    else
#endif /* POLARSSL_GCM_C || POLARSSL_CCM_C */
#if defined(POLARSSL_CIPHER_MODE_CBC) &&                                    \
    ( defined(POLARSSL_AES_C) || defined(POLARSSL_CAMELLIA_C) )
    if( mode == POLARSSL_MODE_CBC )
    {
        /*
         * Decrypt and check the padding
         */
        int ret;
        unsigned char *dec_msg;
        unsigned char *dec_msg_result;
        size_t dec_msglen;
        size_t minlen = 0;
        size_t olen = 0;

        /*
         * Check immediate ciphertext sanity
         */
#if defined(POLARSSL_SSL_PROTO_TLS1_1) || defined(POLARSSL_SSL_PROTO_TLS1_2)
        if( ssl->minor_ver >= SSL_MINOR_VERSION_2 )
            minlen += ssl->transform_in->ivlen;
#endif

        if( ssl->in_msglen < minlen + ssl->transform_in->ivlen ||
            ssl->in_msglen < minlen + ssl->transform_in->maclen + 1 )
        {
            SSL_DEBUG_MSG( 1, ( "msglen (%d) < max( ivlen(%d), maclen (%d) "
                                "+ 1 ) ( + expl IV )", ssl->in_msglen,
                                ssl->transform_in->ivlen,
                                ssl->transform_in->maclen ) );
            return( POLARSSL_ERR_SSL_INVALID_MAC );
        }

        dec_msglen = ssl->in_msglen;
        dec_msg = ssl->in_msg;
        dec_msg_result = ssl->in_msg;

        /*
         * Authenticate before decrypt if enabled
         */
#if defined(POLARSSL_SSL_ENCRYPT_THEN_MAC)
        if( ssl->session_in->encrypt_then_mac == SSL_ETM_ENABLED )
        {
            unsigned char computed_mac[POLARSSL_SSL_MAX_MAC_SIZE];
            unsigned char pseudo_hdr[13];

            SSL_DEBUG_MSG( 3, ( "using encrypt then mac" ) );

            dec_msglen -= ssl->transform_in->maclen;
            ssl->in_msglen -= ssl->transform_in->maclen;

            memcpy( pseudo_hdr +  0, ssl->in_ctr, 8 );
            memcpy( pseudo_hdr +  8, ssl->in_hdr, 3 );
            pseudo_hdr[11] = (unsigned char)( ( ssl->in_msglen >> 8 ) & 0xFF );
            pseudo_hdr[12] = (unsigned char)( ( ssl->in_msglen      ) & 0xFF );

            SSL_DEBUG_BUF( 4, "MAC'd meta-data", pseudo_hdr, 13 );

            md_hmac_update( &ssl->transform_in->md_ctx_dec, pseudo_hdr, 13 );
            md_hmac_update( &ssl->transform_in->md_ctx_dec,
                             ssl->in_iv, ssl->in_msglen );
            md_hmac_finish( &ssl->transform_in->md_ctx_dec, computed_mac );
            md_hmac_reset( &ssl->transform_in->md_ctx_dec );

            SSL_DEBUG_BUF( 4, "message  mac", ssl->in_iv + ssl->in_msglen,
                                              ssl->transform_in->maclen );
            SSL_DEBUG_BUF( 4, "computed mac", computed_mac,
                                              ssl->transform_in->maclen );

            if( safer_memcmp( ssl->in_iv + ssl->in_msglen, computed_mac,
                              ssl->transform_in->maclen ) != 0 )
            {
                SSL_DEBUG_MSG( 1, ( "message mac does not match" ) );

                return( POLARSSL_ERR_SSL_INVALID_MAC );
            }
            auth_done++;
        }
#endif /* POLARSSL_SSL_ENCRYPT_THEN_MAC */

        /*
         * Check length sanity
         */
        if( ssl->in_msglen % ssl->transform_in->ivlen != 0 )
        {
            SSL_DEBUG_MSG( 1, ( "msglen (%d) %% ivlen (%d) != 0",
                           ssl->in_msglen, ssl->transform_in->ivlen ) );
            return( POLARSSL_ERR_SSL_INVALID_MAC );
        }

#if defined(POLARSSL_SSL_PROTO_TLS1_1) || defined(POLARSSL_SSL_PROTO_TLS1_2)
        /*
         * Initialize for prepended IV for block cipher in TLS v1.1 and up
         */
        if( ssl->minor_ver >= SSL_MINOR_VERSION_2 )
        {
            dec_msglen -= ssl->transform_in->ivlen;
            ssl->in_msglen -= ssl->transform_in->ivlen;

            for( i = 0; i < ssl->transform_in->ivlen; i++ )
                ssl->transform_in->iv_dec[i] = ssl->in_iv[i];
        }
#endif /* POLARSSL_SSL_PROTO_TLS1_1 || POLARSSL_SSL_PROTO_TLS1_2 */

        if( ( ret = cipher_crypt( &ssl->transform_in->cipher_ctx_dec,
                                   ssl->transform_in->iv_dec,
                                   ssl->transform_in->ivlen,
                                   dec_msg, dec_msglen,
                                   dec_msg_result, &olen ) ) != 0 )
        {
            SSL_DEBUG_RET( 1, "cipher_crypt", ret );
            return( ret );
        }

        if( dec_msglen != olen )
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
            memcpy( ssl->transform_in->iv_dec,
                    ssl->transform_in->cipher_ctx_dec.iv,
                    ssl->transform_in->ivlen );
        }
#endif

        padlen = 1 + ssl->in_msg[ssl->in_msglen - 1];

        if( ssl->in_msglen < ssl->transform_in->maclen + padlen &&
            auth_done == 0 )
        {
#if defined(POLARSSL_SSL_DEBUG_ALL)
            SSL_DEBUG_MSG( 1, ( "msglen (%d) < maclen (%d) + padlen (%d)",
                        ssl->in_msglen, ssl->transform_in->maclen, padlen ) );
#endif
            padlen = 0;
            correct = 0;
        }

#if defined(POLARSSL_SSL_PROTO_SSL3)
        if( ssl->minor_ver == SSL_MINOR_VERSION_0 )
        {
            if( padlen > ssl->transform_in->ivlen )
            {
#if defined(POLARSSL_SSL_DEBUG_ALL)
                SSL_DEBUG_MSG( 1, ( "bad padding length: is %d, "
                                    "should be no more than %d",
                               padlen, ssl->transform_in->ivlen ) );
#endif
                correct = 0;
            }
        }
        else
#endif /* POLARSSL_SSL_PROTO_SSL3 */
#if defined(POLARSSL_SSL_PROTO_TLS1) || defined(POLARSSL_SSL_PROTO_TLS1_1) || \
    defined(POLARSSL_SSL_PROTO_TLS1_2)
        if( ssl->minor_ver > SSL_MINOR_VERSION_0 )
        {
            /*
             * TLSv1+: always check the padding up to the first failure
             * and fake check up to 256 bytes of padding
             */
            size_t pad_count = 0, real_count = 1;
            size_t padding_idx = ssl->in_msglen - padlen - 1;

            /*
             * Padding is guaranteed to be incorrect if:
             *   1. padlen >= ssl->in_msglen
             *
             *   2. padding_idx >= SSL_MAX_CONTENT_LEN +
             *                     ssl->transform_in->maclen
             *
             * In both cases we reset padding_idx to a safe value (0) to
             * prevent out-of-buffer reads.
             */
            correct &= ( ssl->in_msglen >= padlen + 1 );
            correct &= ( padding_idx < SSL_MAX_CONTENT_LEN +
                                       ssl->transform_in->maclen );

            padding_idx *= correct;

            for( i = 1; i <= 256; i++ )
            {
                real_count &= ( i <= padlen );
                pad_count += real_count *
                             ( ssl->in_msg[padding_idx + i] == padlen - 1 );
            }

            correct &= ( pad_count == padlen ); /* Only 1 on correct padding */

#if defined(POLARSSL_SSL_DEBUG_ALL)
            if( padlen > 0 && correct == 0 )
                SSL_DEBUG_MSG( 1, ( "bad padding byte detected" ) );
#endif
            padlen &= correct * 0x1FF;
        }
        else
#endif /* POLARSSL_SSL_PROTO_TLS1 || POLARSSL_SSL_PROTO_TLS1_1 || \
          POLARSSL_SSL_PROTO_TLS1_2 */
        {
            SSL_DEBUG_MSG( 1, ( "should never happen" ) );
            return( POLARSSL_ERR_SSL_INTERNAL_ERROR );
        }

        ssl->in_msglen -= padlen;
    }
    else
#endif /* POLARSSL_CIPHER_MODE_CBC &&
          ( POLARSSL_AES_C || POLARSSL_CAMELLIA_C ) */
    {
        SSL_DEBUG_MSG( 1, ( "should never happen" ) );
        return( POLARSSL_ERR_SSL_INTERNAL_ERROR );
    }

    SSL_DEBUG_BUF( 4, "raw buffer after decryption",
                   ssl->in_msg, ssl->in_msglen );

    /*
     * Authenticate if not done yet.
     * Compute the MAC regardless of the padding result (RFC4346, CBCTIME).
     */
#if defined(POLARSSL_SOME_MODES_USE_MAC)
    if( auth_done == 0 )
    {
        unsigned char tmp[POLARSSL_SSL_MAX_MAC_SIZE];

        ssl->in_msglen -= ssl->transform_in->maclen;

        ssl->in_hdr[3] = (unsigned char)( ssl->in_msglen >> 8 );
        ssl->in_hdr[4] = (unsigned char)( ssl->in_msglen      );

        memcpy( tmp, ssl->in_msg + ssl->in_msglen, ssl->transform_in->maclen );

#if defined(POLARSSL_SSL_PROTO_SSL3)
        if( ssl->minor_ver == SSL_MINOR_VERSION_0 )
        {
            ssl_mac( &ssl->transform_in->md_ctx_dec,
                      ssl->transform_in->mac_dec,
                      ssl->in_msg, ssl->in_msglen,
                      ssl->in_ctr, ssl->in_msgtype );
        }
        else
#endif /* POLARSSL_SSL_PROTO_SSL3 */
#if defined(POLARSSL_SSL_PROTO_TLS1) || defined(POLARSSL_SSL_PROTO_TLS1_1) || \
        defined(POLARSSL_SSL_PROTO_TLS1_2)
        if( ssl->minor_ver > SSL_MINOR_VERSION_0 )
        {
            /*
             * Process MAC and always update for padlen afterwards to make
             * total time independent of padlen
             *
             * extra_run compensates MAC check for padlen
             *
             * Known timing attacks:
             *  - Lucky Thirteen (http://www.isg.rhul.ac.uk/tls/TLStiming.pdf)
             *
             * We use ( ( Lx + 8 ) / 64 ) to handle 'negative Lx' values
             * correctly. (We round down instead of up, so -56 is the correct
             * value for our calculations instead of -55)
             */
            size_t j, extra_run = 0;
            extra_run = ( 13 + ssl->in_msglen + padlen + 8 ) / 64 -
                        ( 13 + ssl->in_msglen          + 8 ) / 64;

            extra_run &= correct * 0xFF;

            md_hmac_update( &ssl->transform_in->md_ctx_dec, ssl->in_ctr, 13 );
            md_hmac_update( &ssl->transform_in->md_ctx_dec, ssl->in_msg,
                             ssl->in_msglen );
            md_hmac_finish( &ssl->transform_in->md_ctx_dec,
                             ssl->in_msg + ssl->in_msglen );
            /* Call md_process at least once due to cache attacks */
            for( j = 0; j < extra_run + 1; j++ )
                md_process( &ssl->transform_in->md_ctx_dec, ssl->in_msg );

            md_hmac_reset( &ssl->transform_in->md_ctx_dec );
        }
        else
#endif /* POLARSSL_SSL_PROTO_TLS1 || POLARSSL_SSL_PROTO_TLS1_1 || \
              POLARSSL_SSL_PROTO_TLS1_2 */
        {
            SSL_DEBUG_MSG( 1, ( "should never happen" ) );
            return( POLARSSL_ERR_SSL_INTERNAL_ERROR );
        }

        SSL_DEBUG_BUF( 4, "message  mac", tmp, ssl->transform_in->maclen );
        SSL_DEBUG_BUF( 4, "computed mac", ssl->in_msg + ssl->in_msglen,
                       ssl->transform_in->maclen );

        if( safer_memcmp( tmp, ssl->in_msg + ssl->in_msglen,
                         ssl->transform_in->maclen ) != 0 )
        {
#if defined(POLARSSL_SSL_DEBUG_ALL)
            SSL_DEBUG_MSG( 1, ( "message mac does not match" ) );
#endif
            correct = 0;
        }
        auth_done++;

        /*
         * Finally check the correct flag
         */
        if( correct == 0 )
            return( POLARSSL_ERR_SSL_INVALID_MAC );
    }
#endif /* POLARSSL_SOME_MODES_USE_MAC */

    /* Make extra sure authentication was performed, exactly once */
    if( auth_done != 1 )
    {
        SSL_DEBUG_MSG( 1, ( "should never happen" ) );
        return( POLARSSL_ERR_SSL_INTERNAL_ERROR );
    }

    if( ssl->in_msglen == 0 )
    {
        ssl->nb_zero++;

        /*
         * Three or more empty messages may be a DoS attack
         * (excessive CPU consumption).
         */
        if( ssl->nb_zero > 3 )
        {
            SSL_DEBUG_MSG( 1, ( "received four consecutive empty "
                                "messages, possible DoS attack" ) );
            return( POLARSSL_ERR_SSL_INVALID_MAC );
        }
    }
    else
        ssl->nb_zero = 0;

    for( i = 8; i > 0; i-- )
        if( ++ssl->in_ctr[i - 1] != 0 )
            break;

    /* The loops goes to its end iff the counter is wrapping */
    if( i == 0 )
    {
        SSL_DEBUG_MSG( 1, ( "incoming message counter would wrap" ) );
        return( POLARSSL_ERR_SSL_COUNTER_WRAPPING );
    }

    SSL_DEBUG_MSG( 2, ( "<= decrypt buf" ) );

    return( 0 );
}