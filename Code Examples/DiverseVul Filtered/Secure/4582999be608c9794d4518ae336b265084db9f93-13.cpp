int ssl_handshake_init( ssl_context *ssl )
{
    if( ssl->transform_negotiate )
        ssl_transform_free( ssl->transform_negotiate );
    else
        ssl->transform_negotiate = malloc( sizeof(ssl_transform) );

    if( ssl->session_negotiate )
        ssl_session_free( ssl->session_negotiate );
    else
        ssl->session_negotiate = malloc( sizeof(ssl_session) );

    if( ssl->handshake )
        ssl_handshake_free( ssl->handshake );
    else
        ssl->handshake = malloc( sizeof(ssl_handshake_params) );

    if( ssl->handshake == NULL ||
        ssl->transform_negotiate == NULL ||
        ssl->session_negotiate == NULL )
    {
        SSL_DEBUG_MSG( 1, ( "malloc() of ssl sub-contexts failed" ) );
        return( POLARSSL_ERR_SSL_MALLOC_FAILED );
    }

    memset( ssl->handshake, 0, sizeof(ssl_handshake_params) );
    memset( ssl->transform_negotiate, 0, sizeof(ssl_transform) );
    memset( ssl->session_negotiate, 0, sizeof(ssl_session) );

     md5_starts( &ssl->handshake->fin_md5 );
    sha1_starts( &ssl->handshake->fin_sha1 );
    sha2_starts( &ssl->handshake->fin_sha2, 0 );
#if defined(POLARSSL_SHA4_C)
    sha4_starts( &ssl->handshake->fin_sha4, 1 );
#endif

    ssl->handshake->update_checksum = ssl_update_checksum_start;
    ssl->handshake->sig_alg = SSL_HASH_SHA1;
    
    return( 0 );
}