int ssl_read( ssl_context *ssl, unsigned char *buf, size_t len )
{
    int ret;
    size_t n;

    SSL_DEBUG_MSG( 2, ( "=> read" ) );

    if( ssl->state != SSL_HANDSHAKE_OVER )
    {
        if( ( ret = ssl_handshake( ssl ) ) != 0 )
        {
            SSL_DEBUG_RET( 1, "ssl_handshake", ret );
            return( ret );
        }
    }

    if( ssl->in_offt == NULL )
    {
        if( ( ret = ssl_read_record( ssl ) ) != 0 )
        {
            if( ret == POLARSSL_ERR_SSL_CONN_EOF )
                return( 0 );

            SSL_DEBUG_RET( 1, "ssl_read_record", ret );
            return( ret );
        }

        if( ssl->in_msglen  == 0 &&
            ssl->in_msgtype == SSL_MSG_APPLICATION_DATA )
        {
            /*
             * OpenSSL sends empty messages to randomize the IV
             */
            if( ( ret = ssl_read_record( ssl ) ) != 0 )
            {
                if( ret == POLARSSL_ERR_SSL_CONN_EOF )
                    return( 0 );

                SSL_DEBUG_RET( 1, "ssl_read_record", ret );
                return( ret );
            }
        }

        if( ssl->in_msgtype == SSL_MSG_HANDSHAKE )
        {
            SSL_DEBUG_MSG( 1, ( "received handshake message" ) );

            if( ssl->endpoint == SSL_IS_CLIENT &&
                ( ssl->in_msg[0] != SSL_HS_HELLO_REQUEST ||
                  ssl->in_hslen != 4 ) )
            {
                SSL_DEBUG_MSG( 1, ( "handshake received (not HelloRequest)" ) );
                return( POLARSSL_ERR_SSL_UNEXPECTED_MESSAGE );
            }

            if( ssl->disable_renegotiation == SSL_RENEGOTIATION_DISABLED ||
                ( ssl->secure_renegotiation == SSL_LEGACY_RENEGOTIATION &&
                  ssl->allow_legacy_renegotiation == SSL_LEGACY_NO_RENEGOTIATION ) )
            {
                SSL_DEBUG_MSG( 3, ( "ignoring renegotiation, sending alert" ) );

                if( ssl->minor_ver == SSL_MINOR_VERSION_0 )
                {
                    /*
                     * SSLv3 does not have a "no_renegotiation" alert
                     */
                    if( ( ret = ssl_send_fatal_handshake_failure( ssl ) ) != 0 )
                        return( ret );
                }
                else
                {
                    if( ( ret = ssl_send_alert_message( ssl,
                                    SSL_ALERT_LEVEL_WARNING,
                                    SSL_ALERT_MSG_NO_RENEGOTIATION ) ) != 0 )
                    {
                        return( ret );
                    }
                }
            }
            else
            {
                if( ( ret = ssl_renegotiate( ssl ) ) != 0 )
                {
                    SSL_DEBUG_RET( 1, "ssl_renegotiate", ret );
                    return( ret );
                }

                return( POLARSSL_ERR_NET_WANT_READ );
            }
        }
        else if( ssl->in_msgtype != SSL_MSG_APPLICATION_DATA )
        {
            SSL_DEBUG_MSG( 1, ( "bad application data message" ) );
            return( POLARSSL_ERR_SSL_UNEXPECTED_MESSAGE );
        }

        ssl->in_offt = ssl->in_msg;
    }

    n = ( len < ssl->in_msglen )
        ? len : ssl->in_msglen;

    memcpy( buf, ssl->in_offt, n );
    ssl->in_msglen -= n;

    if( ssl->in_msglen == 0 )
        /* all bytes consumed  */
        ssl->in_offt = NULL;
    else
        /* more data available */
        ssl->in_offt += n;

    SSL_DEBUG_MSG( 2, ( "<= read" ) );

    return( (int) n );
}