int ssl_read_record( ssl_context *ssl )
{
    int ret, done = 0;

    SSL_DEBUG_MSG( 2, ( "=> read record" ) );

    if( ssl->in_hslen != 0 &&
        ssl->in_hslen < ssl->in_msglen )
    {
        /*
         * Get next Handshake message in the current record
         */
        ssl->in_msglen -= ssl->in_hslen;

        memmove( ssl->in_msg, ssl->in_msg + ssl->in_hslen,
                 ssl->in_msglen );

        ssl->in_hslen  = 4;
        ssl->in_hslen += ( ssl->in_msg[2] << 8 ) | ssl->in_msg[3];

        SSL_DEBUG_MSG( 3, ( "handshake message: msglen ="
                            " %d, type = %d, hslen = %d",
                       ssl->in_msglen, ssl->in_msg[0], ssl->in_hslen ) );

        if( ssl->in_msglen < 4 || ssl->in_msg[1] != 0 )
        {
            SSL_DEBUG_MSG( 1, ( "bad handshake length" ) );
            return( POLARSSL_ERR_SSL_INVALID_RECORD );
        }

        if( ssl->in_msglen < ssl->in_hslen )
        {
            SSL_DEBUG_MSG( 1, ( "bad handshake length" ) );
            return( POLARSSL_ERR_SSL_INVALID_RECORD );
        }

        if( ssl->state != SSL_HANDSHAKE_OVER )
            ssl->handshake->update_checksum( ssl, ssl->in_msg, ssl->in_hslen );

        return( 0 );
    }

    ssl->in_hslen = 0;

    /*
     * Read the record header and validate it
     */
    if( ( ret = ssl_fetch_input( ssl, 5 ) ) != 0 )
    {
        SSL_DEBUG_RET( 1, "ssl_fetch_input", ret );
        return( ret );
    }

    ssl->in_msgtype =  ssl->in_hdr[0];
    ssl->in_msglen = ( ssl->in_hdr[3] << 8 ) | ssl->in_hdr[4];

    SSL_DEBUG_MSG( 3, ( "input record: msgtype = %d, "
                        "version = [%d:%d], msglen = %d",
                     ssl->in_hdr[0], ssl->in_hdr[1], ssl->in_hdr[2],
                   ( ssl->in_hdr[3] << 8 ) | ssl->in_hdr[4] ) );

    if( ssl->in_hdr[1] != ssl->major_ver )
    {
        SSL_DEBUG_MSG( 1, ( "major version mismatch" ) );
        return( POLARSSL_ERR_SSL_INVALID_RECORD );
    }

    if( ssl->in_hdr[2] > ssl->max_minor_ver )
    {
        SSL_DEBUG_MSG( 1, ( "minor version mismatch" ) );
        return( POLARSSL_ERR_SSL_INVALID_RECORD );
    }

    /* Sanity check (outer boundaries) */
    if( ssl->in_msglen < 1 || ssl->in_msglen > SSL_BUFFER_LEN - 13 )
    {
        SSL_DEBUG_MSG( 1, ( "bad message length" ) );
        return( POLARSSL_ERR_SSL_INVALID_RECORD );
    }

    /*
     * Make sure the message length is acceptable for the current transform
     * and protocol version.
     */
    if( ssl->transform_in == NULL )
    {
        if( ssl->in_msglen > SSL_MAX_CONTENT_LEN )
        {
            SSL_DEBUG_MSG( 1, ( "bad message length" ) );
            return( POLARSSL_ERR_SSL_INVALID_RECORD );
        }
    }
    else
    {
        if( ssl->in_msglen < ssl->transform_in->minlen )
        {
            SSL_DEBUG_MSG( 1, ( "bad message length" ) );
            return( POLARSSL_ERR_SSL_INVALID_RECORD );
        }

#if defined(POLARSSL_SSL_PROTO_SSL3)
        if( ssl->minor_ver == SSL_MINOR_VERSION_0 &&
            ssl->in_msglen > ssl->transform_in->minlen + SSL_MAX_CONTENT_LEN )
        {
            SSL_DEBUG_MSG( 1, ( "bad message length" ) );
            return( POLARSSL_ERR_SSL_INVALID_RECORD );
        }
#endif

#if defined(POLARSSL_SSL_PROTO_TLS1) || defined(POLARSSL_SSL_PROTO_TLS1_1) || \
    defined(POLARSSL_SSL_PROTO_TLS1_2)
        /*
         * TLS encrypted messages can have up to 256 bytes of padding
         */
        if( ssl->minor_ver >= SSL_MINOR_VERSION_1 &&
            ssl->in_msglen > ssl->transform_in->minlen +
                             SSL_MAX_CONTENT_LEN + 256 )
        {
            SSL_DEBUG_MSG( 1, ( "bad message length" ) );
            return( POLARSSL_ERR_SSL_INVALID_RECORD );
        }
#endif
    }

    /*
     * Read and optionally decrypt the message contents
     */
    if( ( ret = ssl_fetch_input( ssl, 5 + ssl->in_msglen ) ) != 0 )
    {
        SSL_DEBUG_RET( 1, "ssl_fetch_input", ret );
        return( ret );
    }

    SSL_DEBUG_BUF( 4, "input record from network",
                   ssl->in_hdr, 5 + ssl->in_msglen );

#if defined(POLARSSL_SSL_HW_RECORD_ACCEL)
    if( ssl_hw_record_read != NULL )
    {
        SSL_DEBUG_MSG( 2, ( "going for ssl_hw_record_read()" ) );

        ret = ssl_hw_record_read( ssl );
        if( ret != 0 && ret != POLARSSL_ERR_SSL_HW_ACCEL_FALLTHROUGH )
        {
            SSL_DEBUG_RET( 1, "ssl_hw_record_read", ret );
            return( POLARSSL_ERR_SSL_HW_ACCEL_FAILED );
        }

        if( ret == 0 )
            done = 1;
    }
#endif /* POLARSSL_SSL_HW_RECORD_ACCEL */
    if( !done && ssl->transform_in != NULL )
    {
        if( ( ret = ssl_decrypt_buf( ssl ) ) != 0 )
        {
#if defined(POLARSSL_SSL_ALERT_MESSAGES)
            if( ret == POLARSSL_ERR_SSL_INVALID_MAC )
            {
                ssl_send_alert_message( ssl,
                                        SSL_ALERT_LEVEL_FATAL,
                                        SSL_ALERT_MSG_BAD_RECORD_MAC );
            }
#endif
            SSL_DEBUG_RET( 1, "ssl_decrypt_buf", ret );
            return( ret );
        }

        SSL_DEBUG_BUF( 4, "input payload after decrypt",
                       ssl->in_msg, ssl->in_msglen );

        if( ssl->in_msglen > SSL_MAX_CONTENT_LEN )
        {
            SSL_DEBUG_MSG( 1, ( "bad message length" ) );
            return( POLARSSL_ERR_SSL_INVALID_RECORD );
        }
    }

#if defined(POLARSSL_ZLIB_SUPPORT)
    if( ssl->transform_in != NULL &&
        ssl->session_in->compression == SSL_COMPRESS_DEFLATE )
    {
        if( ( ret = ssl_decompress_buf( ssl ) ) != 0 )
        {
            SSL_DEBUG_RET( 1, "ssl_decompress_buf", ret );
            return( ret );
        }

        ssl->in_hdr[3] = (unsigned char)( ssl->in_msglen >> 8 );
        ssl->in_hdr[4] = (unsigned char)( ssl->in_msglen      );
    }
#endif /* POLARSSL_ZLIB_SUPPORT */

    if( ssl->in_msgtype != SSL_MSG_HANDSHAKE &&
        ssl->in_msgtype != SSL_MSG_ALERT &&
        ssl->in_msgtype != SSL_MSG_CHANGE_CIPHER_SPEC &&
        ssl->in_msgtype != SSL_MSG_APPLICATION_DATA )
    {
        SSL_DEBUG_MSG( 1, ( "unknown record type" ) );

        if( ( ret = ssl_send_alert_message( ssl,
                        SSL_ALERT_LEVEL_FATAL,
                        SSL_ALERT_MSG_UNEXPECTED_MESSAGE ) ) != 0 )
        {
            return( ret );
        }

        return( POLARSSL_ERR_SSL_INVALID_RECORD );
    }

    if( ssl->in_msgtype == SSL_MSG_HANDSHAKE )
    {
        ssl->in_hslen  = 4;
        ssl->in_hslen += ( ssl->in_msg[2] << 8 ) | ssl->in_msg[3];

        SSL_DEBUG_MSG( 3, ( "handshake message: msglen ="
                            " %d, type = %d, hslen = %d",
                       ssl->in_msglen, ssl->in_msg[0], ssl->in_hslen ) );

        /*
         * Additional checks to validate the handshake header
         */
        if( ssl->in_msglen < 4 || ssl->in_msg[1] != 0 )
        {
            SSL_DEBUG_MSG( 1, ( "bad handshake length" ) );
            return( POLARSSL_ERR_SSL_INVALID_RECORD );
        }

        if( ssl->in_msglen < ssl->in_hslen )
        {
            SSL_DEBUG_MSG( 1, ( "bad handshake length" ) );
            return( POLARSSL_ERR_SSL_INVALID_RECORD );
        }

        if( ssl->state != SSL_HANDSHAKE_OVER )
            ssl->handshake->update_checksum( ssl, ssl->in_msg, ssl->in_hslen );
    }

    if( ssl->in_msgtype == SSL_MSG_ALERT )
    {
        SSL_DEBUG_MSG( 2, ( "got an alert message, type: [%d:%d]",
                       ssl->in_msg[0], ssl->in_msg[1] ) );

        /*
         * Ignore non-fatal alerts, except close_notify
         */
        if( ssl->in_msg[0] == SSL_ALERT_LEVEL_FATAL )
        {
            SSL_DEBUG_MSG( 1, ( "is a fatal alert message (msg %d)",
                           ssl->in_msg[1] ) );
            return( POLARSSL_ERR_SSL_FATAL_ALERT_MESSAGE );
        }

        if( ssl->in_msg[0] == SSL_ALERT_LEVEL_WARNING &&
            ssl->in_msg[1] == SSL_ALERT_MSG_CLOSE_NOTIFY )
        {
            SSL_DEBUG_MSG( 2, ( "is a close notify message" ) );
            return( POLARSSL_ERR_SSL_PEER_CLOSE_NOTIFY );
        }
    }

    ssl->in_left = 0;

    SSL_DEBUG_MSG( 2, ( "<= read record" ) );

    return( 0 );
}