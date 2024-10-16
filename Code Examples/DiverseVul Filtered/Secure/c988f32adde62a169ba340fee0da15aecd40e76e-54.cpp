static int ssl_start_renegotiation( ssl_context *ssl )
{
    int ret;

    SSL_DEBUG_MSG( 2, ( "=> renegotiate" ) );

    if( ( ret = ssl_handshake_init( ssl ) ) != 0 )
        return( ret );

    ssl->state = SSL_HELLO_REQUEST;
    ssl->renegotiation = SSL_RENEGOTIATION;

    if( ( ret = ssl_handshake( ssl ) ) != 0 )
    {
        SSL_DEBUG_RET( 1, "ssl_handshake", ret );
        return( ret );
    }

    SSL_DEBUG_MSG( 2, ( "<= renegotiate" ) );

    return( 0 );
}