int ssl_send_alert_message( ssl_context *ssl,
                            unsigned char level,
                            unsigned char message )
{
    int ret;

    SSL_DEBUG_MSG( 2, ( "=> send alert message" ) );

    ssl->out_msgtype = SSL_MSG_ALERT;
    ssl->out_msglen = 2;
    ssl->out_msg[0] = level;
    ssl->out_msg[1] = message;

    if( ( ret = ssl_write_record( ssl ) ) != 0 )
    {
        SSL_DEBUG_RET( 1, "ssl_write_record", ret );
        return( ret );
    }

    SSL_DEBUG_MSG( 2, ( "<= send alert message" ) );

    return( 0 );
}