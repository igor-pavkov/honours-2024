int ssl_write( ssl_context *ssl, const unsigned char *buf, size_t len )
{
    int ret;

    SSL_DEBUG_MSG( 2, ( "=> write" ) );

#if defined(POLARSSL_SSL_RENEGOTIATION)
    if( ( ret = ssl_check_ctr_renegotiate( ssl ) ) != 0 )
    {
        SSL_DEBUG_RET( 1, "ssl_check_ctr_renegotiate", ret );
        return( ret );
    }
#endif

    if( ssl->state != SSL_HANDSHAKE_OVER )
    {
        if( ( ret = ssl_handshake( ssl ) ) != 0 )
        {
            SSL_DEBUG_RET( 1, "ssl_handshake", ret );
            return( ret );
        }
    }

#if defined(POLARSSL_SSL_CBC_RECORD_SPLITTING)
    ret = ssl_write_split( ssl, buf, len );
#else
    ret = ssl_write_real( ssl, buf, len );
#endif

    SSL_DEBUG_MSG( 2, ( "<= write" ) );

    return( ret );
}