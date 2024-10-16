int ssl_init( ssl_context *ssl )
{
    int ret;
    int len = SSL_BUFFER_LEN;

    memset( ssl, 0, sizeof( ssl_context ) );

    /*
     * Sane defaults
     */
    ssl->min_major_ver = SSL_MIN_MAJOR_VERSION;
    ssl->min_minor_ver = SSL_MIN_MINOR_VERSION;
    ssl->max_major_ver = SSL_MAX_MAJOR_VERSION;
    ssl->max_minor_ver = SSL_MAX_MINOR_VERSION;

    ssl_set_ciphersuites( ssl, ssl_list_ciphersuites() );

#if defined(POLARSSL_SSL_RENEGOTIATION)
    ssl->renego_max_records = SSL_RENEGO_MAX_RECORDS_DEFAULT;
    memset( ssl->renego_period, 0xFF, 7 );
    ssl->renego_period[7] = 0x00;
#endif

#if defined(POLARSSL_DHM_C)
    if( ( ret = mpi_read_string( &ssl->dhm_P, 16,
                                 POLARSSL_DHM_RFC5114_MODP_2048_P) ) != 0 ||
        ( ret = mpi_read_string( &ssl->dhm_G, 16,
                                 POLARSSL_DHM_RFC5114_MODP_2048_G) ) != 0 )
    {
        SSL_DEBUG_RET( 1, "mpi_read_string", ret );
        return( ret );
    }
#endif

    /*
     * Prepare base structures
     */
    if( ( ssl->in_ctr = polarssl_malloc( len ) ) == NULL ||
        ( ssl->out_ctr = polarssl_malloc( len ) ) == NULL )
    {
        SSL_DEBUG_MSG( 1, ( "malloc(%d bytes) failed", len ) );
        polarssl_free( ssl->in_ctr );
        ssl->in_ctr = NULL;
        return( POLARSSL_ERR_SSL_MALLOC_FAILED );
    }

    memset( ssl-> in_ctr, 0, SSL_BUFFER_LEN );
    memset( ssl->out_ctr, 0, SSL_BUFFER_LEN );

    ssl->in_hdr = ssl->in_ctr +  8;
    ssl->in_iv  = ssl->in_ctr + 13;
    ssl->in_msg = ssl->in_ctr + 13;

    ssl->out_hdr = ssl->out_ctr +  8;
    ssl->out_iv  = ssl->out_ctr + 13;
    ssl->out_msg = ssl->out_ctr + 13;

#if defined(POLARSSL_SSL_ENCRYPT_THEN_MAC)
    ssl->encrypt_then_mac = SSL_ETM_ENABLED;
#endif

#if defined(POLARSSL_SSL_EXTENDED_MASTER_SECRET)
    ssl->extended_ms = SSL_EXTENDED_MS_ENABLED;
#endif

#if defined(POLARSSL_SSL_SESSION_TICKETS)
    ssl->ticket_lifetime = SSL_DEFAULT_TICKET_LIFETIME;
#endif

#if defined(POLARSSL_SSL_SET_CURVES)
    ssl->curve_list = ecp_grp_id_list( );
#endif

    if( ( ret = ssl_handshake_init( ssl ) ) != 0 )
        return( ret );

    return( 0 );
}