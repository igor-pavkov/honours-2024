void ssl_free( ssl_context *ssl )
{
    SSL_DEBUG_MSG( 2, ( "=> free" ) );

    if( ssl->out_ctr != NULL )
    {
        memset( ssl->out_ctr, 0, SSL_BUFFER_LEN );
          free( ssl->out_ctr );
    }

    if( ssl->in_ctr != NULL )
    {
        memset( ssl->in_ctr, 0, SSL_BUFFER_LEN );
          free( ssl->in_ctr );
    }

#if defined(POLARSSL_DHM_C)
    mpi_free( &ssl->dhm_P );
    mpi_free( &ssl->dhm_G );
#endif

    if( ssl->transform )
    {
        ssl_transform_free( ssl->transform );
        free( ssl->transform );
    }

    if( ssl->handshake )
    {
        ssl_handshake_free( ssl->handshake );
        ssl_transform_free( ssl->transform_negotiate );
        ssl_session_free( ssl->session_negotiate );

        free( ssl->handshake );
        free( ssl->transform_negotiate );
        free( ssl->session_negotiate );
    }

    if ( ssl->hostname != NULL)
    {
        memset( ssl->hostname, 0, ssl->hostname_len );
        free( ssl->hostname );
        ssl->hostname_len = 0;
    }

#if defined(POLARSSL_SSL_HW_RECORD_ACCEL)
    if( ssl_hw_record_finish != NULL )
    {
        SSL_DEBUG_MSG( 2, ( "going for ssl_hw_record_finish()" ) );
        ssl_hw_record_finish( ssl );
    }
#endif

    SSL_DEBUG_MSG( 2, ( "<= free" ) );

    /* Actually free after last debug message */
    memset( ssl, 0, sizeof( ssl_context ) );
}