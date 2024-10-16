int ssl_handshake( ssl_context *ssl )
{
    int ret = 0;

    SSL_DEBUG_MSG( 2, ( "=> handshake" ) );

    while( ssl->state != SSL_HANDSHAKE_OVER )
    {
        ret = ssl_handshake_step( ssl );

        if( ret != 0 )
            break;
    }

    SSL_DEBUG_MSG( 2, ( "<= handshake" ) );

    return( ret );
}