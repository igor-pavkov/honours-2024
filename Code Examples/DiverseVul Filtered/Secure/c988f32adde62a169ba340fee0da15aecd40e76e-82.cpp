int ssl_close_notify( ssl_context *ssl )
{
    int ret;

    SSL_DEBUG_MSG( 2, ( "=> write close notify" ) );

    if( ssl->out_left != 0 )
        return( ssl_flush_output( ssl ) );

    if( ssl->state == SSL_HANDSHAKE_OVER )
    {
        if( ( ret = ssl_send_alert_message( ssl,
                        SSL_ALERT_LEVEL_WARNING,
                        SSL_ALERT_MSG_CLOSE_NOTIFY ) ) != 0 )
        {
            SSL_DEBUG_RET( 1, "ssl_send_alert_message", ret );
            return( ret );
        }
    }

    SSL_DEBUG_MSG( 2, ( "<= write close notify" ) );

    return( 0 );
}