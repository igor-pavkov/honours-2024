int ssl_send_fatal_handshake_failure( ssl_context *ssl )
{
    int ret;

    if( ( ret = ssl_send_alert_message( ssl,
                    SSL_ALERT_LEVEL_FATAL,
                    SSL_ALERT_MSG_HANDSHAKE_FAILURE ) ) != 0 )
    {
        return( ret );
    }

    return( 0 );
}