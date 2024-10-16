void ssl_free( ssl_context *ssl )
{
    if( ssl == NULL )
        return;

    SSL_DEBUG_MSG( 2, ( "=> free" ) );

    if( ssl->out_ctr != NULL )
    {
        polarssl_zeroize( ssl->out_ctr, SSL_BUFFER_LEN );
        polarssl_free( ssl->out_ctr );
    }

    if( ssl->in_ctr != NULL )
    {
        polarssl_zeroize( ssl->in_ctr, SSL_BUFFER_LEN );
        polarssl_free( ssl->in_ctr );
    }

#if defined(POLARSSL_ZLIB_SUPPORT)
    if( ssl->compress_buf != NULL )
    {
        polarssl_zeroize( ssl->compress_buf, SSL_BUFFER_LEN );
        polarssl_free( ssl->compress_buf );
    }
#endif

#if defined(POLARSSL_DHM_C)
    mpi_free( &ssl->dhm_P );
    mpi_free( &ssl->dhm_G );
#endif

    if( ssl->transform )
    {
        ssl_transform_free( ssl->transform );
        polarssl_free( ssl->transform );
    }

    if( ssl->handshake )
    {
        ssl_handshake_free( ssl->handshake );
        ssl_transform_free( ssl->transform_negotiate );
        ssl_session_free( ssl->session_negotiate );

        polarssl_free( ssl->handshake );
        polarssl_free( ssl->transform_negotiate );
        polarssl_free( ssl->session_negotiate );
    }

    if( ssl->session )
    {
        ssl_session_free( ssl->session );
        polarssl_free( ssl->session );
    }

#if defined(POLARSSL_SSL_SESSION_TICKETS)
    if( ssl->ticket_keys )
    {
        ssl_ticket_keys_free( ssl->ticket_keys );
        polarssl_free( ssl->ticket_keys );
    }
#endif

#if defined(POLARSSL_SSL_SERVER_NAME_INDICATION)
    if( ssl->hostname != NULL )
    {
        polarssl_zeroize( ssl->hostname, ssl->hostname_len );
        polarssl_free( ssl->hostname );
        ssl->hostname_len = 0;
    }
#endif

#if defined(POLARSSL_KEY_EXCHANGE__SOME__PSK_ENABLED)
    if( ssl->psk != NULL )
    {
        polarssl_zeroize( ssl->psk, ssl->psk_len );
        polarssl_zeroize( ssl->psk_identity, ssl->psk_identity_len );
        polarssl_free( ssl->psk );
        polarssl_free( ssl->psk_identity );
        ssl->psk_len = 0;
        ssl->psk_identity_len = 0;
    }
#endif

#if defined(POLARSSL_X509_CRT_PARSE_C)
    ssl_key_cert_free( ssl->key_cert );
#endif

#if defined(POLARSSL_SSL_HW_RECORD_ACCEL)
    if( ssl_hw_record_finish != NULL )
    {
        SSL_DEBUG_MSG( 2, ( "going for ssl_hw_record_finish()" ) );
        ssl_hw_record_finish( ssl );
    }
#endif

    SSL_DEBUG_MSG( 2, ( "<= free" ) );

    /* Actually clear after last debug message */
    polarssl_zeroize( ssl, sizeof( ssl_context ) );
}