void ssl_handshake_wrapup( ssl_context *ssl )
{
    SSL_DEBUG_MSG( 3, ( "=> handshake wrapup" ) );

    /*
     * Free our handshake params
     */
    ssl_handshake_free( ssl->handshake );
    free( ssl->handshake );
    ssl->handshake = NULL;

    /*
     * Switch in our now active transform context
     */
    if( ssl->transform )
    {
        ssl_transform_free( ssl->transform );
        free( ssl->transform );
    }
    ssl->transform = ssl->transform_negotiate;
    ssl->transform_negotiate = NULL;

    if( ssl->session )
    {
        ssl_session_free( ssl->session );
        free( ssl->session );
    }
    ssl->session = ssl->session_negotiate;
    ssl->session_negotiate = NULL;

    /*
     * Add cache entry
     */
    if( ssl->f_set_cache != NULL )
        if( ssl->f_set_cache( ssl->p_set_cache, ssl->session ) != 0 )
            SSL_DEBUG_MSG( 1, ( "cache did not store session" ) );

    ssl->state++;

    SSL_DEBUG_MSG( 3, ( "<= handshake wrapup" ) );
}