int ssl_parse_finished( ssl_context *ssl )
{
    int ret;
    unsigned int hash_len;
    unsigned char buf[36];

    SSL_DEBUG_MSG( 2, ( "=> parse finished" ) );

    ssl->handshake->calc_finished( ssl, buf, ssl->endpoint ^ 1 );

    /*
     * Switch to our negotiated transform and session parameters for inbound
     * data.
     */
    SSL_DEBUG_MSG( 3, ( "switching to new transform spec for inbound data" ) );
    ssl->transform_in = ssl->transform_negotiate;
    ssl->session_in = ssl->session_negotiate;
    memset( ssl->in_ctr, 0, 8 );

    /*
     * Set the in_msg pointer to the correct location based on IV length
     */
    if( ssl->minor_ver >= SSL_MINOR_VERSION_2 )
    {
        ssl->in_msg = ssl->in_iv + ssl->transform_negotiate->ivlen -
                      ssl->transform_negotiate->fixed_ivlen;
    }
    else
        ssl->in_msg = ssl->in_iv;

#if defined(POLARSSL_SSL_HW_RECORD_ACCEL)
    if( ssl_hw_record_activate != NULL )
    {
        if( ( ret = ssl_hw_record_activate( ssl, SSL_CHANNEL_INBOUND ) ) != 0 )
        {
            SSL_DEBUG_RET( 1, "ssl_hw_record_activate", ret );
            return( POLARSSL_ERR_SSL_HW_ACCEL_FAILED );
        }
    }
#endif

    if( ( ret = ssl_read_record( ssl ) ) != 0 )
    {
        SSL_DEBUG_RET( 1, "ssl_read_record", ret );
        return( ret );
    }

    if( ssl->in_msgtype != SSL_MSG_HANDSHAKE )
    {
        SSL_DEBUG_MSG( 1, ( "bad finished message" ) );
        return( POLARSSL_ERR_SSL_UNEXPECTED_MESSAGE );
    }

    // TODO TLS/1.2 Hash length is determined by cipher suite (Page 63)
    hash_len = ( ssl->minor_ver == SSL_MINOR_VERSION_0 ) ? 36 : 12;

    if( ssl->in_msg[0] != SSL_HS_FINISHED ||
        ssl->in_hslen  != 4 + hash_len )
    {
        SSL_DEBUG_MSG( 1, ( "bad finished message" ) );
        return( POLARSSL_ERR_SSL_BAD_HS_FINISHED );
    }

    if( safer_memcmp( ssl->in_msg + 4, buf, hash_len ) != 0 )
    {
        SSL_DEBUG_MSG( 1, ( "bad finished message" ) );
        return( POLARSSL_ERR_SSL_BAD_HS_FINISHED );
    }

#if defined(POLARSSL_SSL_RENEGOTIATION)
    ssl->verify_data_len = hash_len;
    memcpy( ssl->peer_verify_data, buf, hash_len );
#endif

    if( ssl->handshake->resume != 0 )
    {
#if defined(POLARSSL_SSL_CLI_C)
        if( ssl->endpoint == SSL_IS_CLIENT )
            ssl->state = SSL_CLIENT_CHANGE_CIPHER_SPEC;
#endif
#if defined(POLARSSL_SSL_SRV_C)
        if( ssl->endpoint == SSL_IS_SERVER )
            ssl->state = SSL_HANDSHAKE_WRAPUP;
#endif
    }
    else
        ssl->state++;

    SSL_DEBUG_MSG( 2, ( "<= parse finished" ) );

    return( 0 );
}