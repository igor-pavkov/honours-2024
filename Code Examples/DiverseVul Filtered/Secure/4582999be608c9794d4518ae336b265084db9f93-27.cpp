int ssl_init( ssl_context *ssl )
{
    int ret;
    int len = SSL_BUFFER_LEN;

    memset( ssl, 0, sizeof( ssl_context ) );

    /*
     * Sane defaults
     */
    ssl->rsa_decrypt = ssl_rsa_decrypt;
    ssl->rsa_sign = ssl_rsa_sign;
    ssl->rsa_key_len = ssl_rsa_key_len;

    ssl->min_major_ver = SSL_MAJOR_VERSION_3;
    ssl->min_minor_ver = SSL_MINOR_VERSION_0;

    ssl->ciphersuites = ssl_default_ciphersuites;

#if defined(POLARSSL_DHM_C)
    if( ( ret = mpi_read_string( &ssl->dhm_P, 16,
                                 POLARSSL_DHM_RFC5114_MODP_1024_P) ) != 0 ||
        ( ret = mpi_read_string( &ssl->dhm_G, 16,
                                 POLARSSL_DHM_RFC5114_MODP_1024_G) ) != 0 )
    {
        SSL_DEBUG_RET( 1, "mpi_read_string", ret );
        return( ret );
    }
#endif

    /*
     * Prepare base structures
     */
    ssl->in_ctr = (unsigned char *) malloc( len );
    ssl->in_hdr = ssl->in_ctr +  8;
    ssl->in_msg = ssl->in_ctr + 13;

    if( ssl->in_ctr == NULL )
    {
        SSL_DEBUG_MSG( 1, ( "malloc(%d bytes) failed", len ) );
        return( POLARSSL_ERR_SSL_MALLOC_FAILED );
    }

    ssl->out_ctr = (unsigned char *) malloc( len );
    ssl->out_hdr = ssl->out_ctr +  8;
    ssl->out_msg = ssl->out_ctr + 40;

    if( ssl->out_ctr == NULL )
    {
        SSL_DEBUG_MSG( 1, ( "malloc(%d bytes) failed", len ) );
        free( ssl-> in_ctr );
        return( POLARSSL_ERR_SSL_MALLOC_FAILED );
    }

    memset( ssl-> in_ctr, 0, SSL_BUFFER_LEN );
    memset( ssl->out_ctr, 0, SSL_BUFFER_LEN );

    ssl->hostname = NULL;
    ssl->hostname_len = 0;

    if( ( ret = ssl_handshake_init( ssl ) ) != 0 )
        return( ret );

    return( 0 );
}