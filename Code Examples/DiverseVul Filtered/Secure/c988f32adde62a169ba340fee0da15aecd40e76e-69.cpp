static int ssl_handshake_init( ssl_context *ssl )
{
    /* Clear old handshake information if present */
    if( ssl->transform_negotiate )
        ssl_transform_free( ssl->transform_negotiate );
    if( ssl->session_negotiate )
        ssl_session_free( ssl->session_negotiate );
    if( ssl->handshake )
        ssl_handshake_free( ssl->handshake );

    /*
     * Either the pointers are now NULL or cleared properly and can be freed.
     * Now allocate missing structures.
     */
    if( ssl->transform_negotiate == NULL )
    {
        ssl->transform_negotiate = polarssl_malloc( sizeof(ssl_transform) );
    }

    if( ssl->session_negotiate == NULL )
    {
        ssl->session_negotiate = polarssl_malloc( sizeof(ssl_session) );
    }

    if( ssl->handshake == NULL )
    {
        ssl->handshake = polarssl_malloc( sizeof(ssl_handshake_params) );
    }

    /* All pointers should exist and can be directly freed without issue */
    if( ssl->handshake == NULL ||
        ssl->transform_negotiate == NULL ||
        ssl->session_negotiate == NULL )
    {
        SSL_DEBUG_MSG( 1, ( "malloc() of ssl sub-contexts failed" ) );

        polarssl_free( ssl->handshake );
        polarssl_free( ssl->transform_negotiate );
        polarssl_free( ssl->session_negotiate );

        ssl->handshake = NULL;
        ssl->transform_negotiate = NULL;
        ssl->session_negotiate = NULL;

        return( POLARSSL_ERR_SSL_MALLOC_FAILED );
    }

    /* Initialize structures */
    ssl_session_init( ssl->session_negotiate );
    ssl_transform_init( ssl->transform_negotiate );
    ssl_handshake_params_init( ssl->handshake );

#if defined(POLARSSL_X509_CRT_PARSE_C)
    ssl->handshake->key_cert = ssl->key_cert;
#endif

    return( 0 );
}