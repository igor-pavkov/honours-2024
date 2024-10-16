int ssl_read( ssl_context *ssl, unsigned char *buf, size_t len )
{
    int ret, record_read = 0;
    size_t n;

    SSL_DEBUG_MSG( 2, ( "=> read" ) );

#if defined(POLARSSL_SSL_RENEGOTIATION)
    if( ( ret = ssl_check_ctr_renegotiate( ssl ) ) != 0 )
    {
        SSL_DEBUG_RET( 1, "ssl_check_ctr_renegotiate", ret );
        return( ret );
    }
#endif

    if( ssl->state != SSL_HANDSHAKE_OVER )
    {
        ret = ssl_handshake( ssl );
        if( ret == POLARSSL_ERR_SSL_WAITING_SERVER_HELLO_RENEGO )
        {
            record_read = 1;
        }
        else if( ret != 0 )
        {
            SSL_DEBUG_RET( 1, "ssl_handshake", ret );
            return( ret );
        }
    }

    if( ssl->in_offt == NULL )
    {
        if( ! record_read )
        {
            if( ( ret = ssl_read_record( ssl ) ) != 0 )
            {
                if( ret == POLARSSL_ERR_SSL_CONN_EOF )
                    return( 0 );

                SSL_DEBUG_RET( 1, "ssl_read_record", ret );
                return( ret );
            }
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

#if defined(POLARSSL_SSL_RENEGOTIATION)
        if( ssl->in_msgtype == SSL_MSG_HANDSHAKE )
        {
            SSL_DEBUG_MSG( 1, ( "received handshake message" ) );

#if defined(POLARSSL_SSL_CLI_C)
            if( ssl->endpoint == SSL_IS_CLIENT &&
                ( ssl->in_msg[0] != SSL_HS_HELLO_REQUEST ||
                  ssl->in_hslen != 4 ) )
            {
                SSL_DEBUG_MSG( 1, ( "handshake received (not HelloRequest)" ) );
                return( POLARSSL_ERR_SSL_UNEXPECTED_MESSAGE );
            }
#endif

            if( ssl->disable_renegotiation == SSL_RENEGOTIATION_DISABLED ||
                ( ssl->secure_renegotiation == SSL_LEGACY_RENEGOTIATION &&
                  ssl->allow_legacy_renegotiation ==
                                                SSL_LEGACY_NO_RENEGOTIATION ) )
            {
                SSL_DEBUG_MSG( 3, ( "ignoring renegotiation, sending alert" ) );

#if defined(POLARSSL_SSL_PROTO_SSL3)
                if( ssl->minor_ver == SSL_MINOR_VERSION_0 )
                {
                    /*
                     * SSLv3 does not have a "no_renegotiation" alert
                     */
                    if( ( ret = ssl_send_fatal_handshake_failure( ssl ) ) != 0 )
                        return( ret );
                }
                else
#endif /* POLARSSL_SSL_PROTO_SSL3 */
#if defined(POLARSSL_SSL_PROTO_TLS1) || defined(POLARSSL_SSL_PROTO_TLS1_1) || \
    defined(POLARSSL_SSL_PROTO_TLS1_2)
                if( ssl->minor_ver >= SSL_MINOR_VERSION_1 )
                {
                    if( ( ret = ssl_send_alert_message( ssl,
                                    SSL_ALERT_LEVEL_WARNING,
                                    SSL_ALERT_MSG_NO_RENEGOTIATION ) ) != 0 )
                    {
                        return( ret );
                    }
                }
                else
#endif /* POLARSSL_SSL_PROTO_TLS1 || POLARSSL_SSL_PROTO_TLS1_1 ||
          POLARSSL_SSL_PROTO_TLS1_2 */
                {
                    SSL_DEBUG_MSG( 1, ( "should never happen" ) );
                    return( POLARSSL_ERR_SSL_INTERNAL_ERROR );
                }
            }
            else
            {
                ret = ssl_start_renegotiation( ssl );
                if( ret == POLARSSL_ERR_SSL_WAITING_SERVER_HELLO_RENEGO )
                {
                    record_read = 1;
                }
                else if( ret != 0 )
                {
                    SSL_DEBUG_RET( 1, "ssl_start_renegotiation", ret );
                    return( ret );
                }
            }

            /* If a non-handshake record was read during renego, fallthrough,
             * else tell the user they should call ssl_read() again */
            if( ! record_read )
                return( POLARSSL_ERR_NET_WANT_READ );
        }
        else if( ssl->renegotiation == SSL_RENEGOTIATION_PENDING )
        {
            ssl->renego_records_seen++;

            if( ssl->renego_max_records >= 0 &&
                ssl->renego_records_seen > ssl->renego_max_records )
            {
                SSL_DEBUG_MSG( 1, ( "renegotiation requested, "
                                    "but not honored by client" ) );
                return( POLARSSL_ERR_SSL_UNEXPECTED_MESSAGE );
            }
        }
#endif /* POLARSSL_SSL_RENEGOTIATION */

        /* Fatal and closure alerts handled by ssl_read_record() */
        if( ssl->in_msgtype == SSL_MSG_ALERT )
        {
            SSL_DEBUG_MSG( 2, ( "ignoring non-fatal non-closure alert" ) );
            return( POLARSSL_ERR_NET_WANT_READ );
        }

        if( ssl->in_msgtype != SSL_MSG_APPLICATION_DATA )
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