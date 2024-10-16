void ssl_handshake_wrapup( ssl_context *ssl )
{
    int resume = ssl->handshake->resume;

    SSL_DEBUG_MSG( 3, ( "=> handshake wrapup" ) );

    /*
     * Free our handshake params
     */
    ssl_handshake_free( ssl->handshake );
    polarssl_free( ssl->handshake );
    ssl->handshake = NULL;

#if defined(POLARSSL_SSL_RENEGOTIATION)
    if( ssl->renegotiation == SSL_RENEGOTIATION )
    {
        ssl->renegotiation =  SSL_RENEGOTIATION_DONE;
        ssl->renego_records_seen = 0;
    }
#endif

    /*
     * Switch in our now active transform context
     */
    if( ssl->transform )
    {
        ssl_transform_free( ssl->transform );
        polarssl_free( ssl->transform );
    }
    ssl->transform = ssl->transform_negotiate;
    ssl->transform_negotiate = NULL;

    if( ssl->session )
    {
#if defined(POLARSSL_SSL_ENCRYPT_THEN_MAC)
        /* RFC 7366 3.1: keep the EtM state */
        ssl->session_negotiate->encrypt_then_mac =
                  ssl->session->encrypt_then_mac;
#endif

        ssl_session_free( ssl->session );
        polarssl_free( ssl->session );
    }
    ssl->session = ssl->session_negotiate;
    ssl->session_negotiate = NULL;

    /*
     * Add cache entry
     */
    if( ssl->f_set_cache != NULL &&
        ssl->session->length != 0 &&
        resume == 0 )
    {
        if( ssl->f_set_cache( ssl->p_set_cache, ssl->session ) != 0 )
            SSL_DEBUG_MSG( 1, ( "cache did not store session" ) );
    }

    ssl->state++;

    SSL_DEBUG_MSG( 3, ( "<= handshake wrapup" ) );
}