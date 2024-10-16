int ssl_renegotiate( ssl_context *ssl )
{
    int ret;

    SSL_DEBUG_MSG( 2, ( "=> renegotiate" ) );

    if( ssl->state != SSL_HANDSHAKE_OVER )
        return( POLARSSL_ERR_SSL_BAD_INPUT_DATA );

    ssl->state = SSL_HELLO_REQUEST;
    ssl->renegotiation = SSL_RENEGOTIATION;

    if( ( ret = ssl_handshake_init( ssl ) ) != 0 )
        return( ret );

    if( ( ret = ssl_handshake( ssl ) ) != 0 )
    {
        SSL_DEBUG_RET( 1, "ssl_handshake", ret );
        return( ret );
    }

    SSL_DEBUG_MSG( 2, ( "<= renegotiate" ) );

    return( 0 );
}