int ssl_renegotiate( ssl_context *ssl )
{
    int ret = POLARSSL_ERR_SSL_FEATURE_UNAVAILABLE;

#if defined(POLARSSL_SSL_SRV_C)
    /* On server, just send the request */
    if( ssl->endpoint == SSL_IS_SERVER )
    {
        if( ssl->state != SSL_HANDSHAKE_OVER )
            return( POLARSSL_ERR_SSL_BAD_INPUT_DATA );

        ssl->renegotiation = SSL_RENEGOTIATION_PENDING;

        /* Did we already try/start sending HelloRequest? */
        if( ssl->out_left != 0 )
            return( ssl_flush_output( ssl ) );

        return( ssl_write_hello_request( ssl ) );
    }
#endif /* POLARSSL_SSL_SRV_C */

#if defined(POLARSSL_SSL_CLI_C)
    /*
     * On client, either start the renegotiation process or,
     * if already in progress, continue the handshake
     */
    if( ssl->renegotiation != SSL_RENEGOTIATION )
    {
        if( ssl->state != SSL_HANDSHAKE_OVER )
            return( POLARSSL_ERR_SSL_BAD_INPUT_DATA );

        if( ( ret = ssl_start_renegotiation( ssl ) ) != 0 )
        {
            SSL_DEBUG_RET( 1, "ssl_start_renegotiation", ret );
            return( ret );
        }
    }
    else
    {
        if( ( ret = ssl_handshake( ssl ) ) != 0 )
        {
            SSL_DEBUG_RET( 1, "ssl_handshake", ret );
            return( ret );
        }
    }
#endif /* POLARSSL_SSL_CLI_C */

    return( ret );
}