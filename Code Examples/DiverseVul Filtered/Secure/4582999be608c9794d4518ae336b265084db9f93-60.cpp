int ssl_write( ssl_context *ssl, const unsigned char *buf, size_t len )
{
    int ret;
    size_t n;

    SSL_DEBUG_MSG( 2, ( "=> write" ) );

    if( ssl->state != SSL_HANDSHAKE_OVER )
    {
        if( ( ret = ssl_handshake( ssl ) ) != 0 )
        {
            SSL_DEBUG_RET( 1, "ssl_handshake", ret );
            return( ret );
        }
    }

    n = ( len < SSL_MAX_CONTENT_LEN )
        ? len : SSL_MAX_CONTENT_LEN;

    if( ssl->out_left != 0 )
    {
        if( ( ret = ssl_flush_output( ssl ) ) != 0 )
        {
            SSL_DEBUG_RET( 1, "ssl_flush_output", ret );
            return( ret );
        }
    }
    else
    {
        ssl->out_msglen  = n;
        ssl->out_msgtype = SSL_MSG_APPLICATION_DATA;
        memcpy( ssl->out_msg, buf, n );

        if( ( ret = ssl_write_record( ssl ) ) != 0 )
        {
            SSL_DEBUG_RET( 1, "ssl_write_record", ret );
            return( ret );
        }
    }

    SSL_DEBUG_MSG( 2, ( "<= write" ) );

    return( (int) n );
}