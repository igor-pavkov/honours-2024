int ssl_write_finished( ssl_context *ssl )
{
    int ret, hash_len;

    SSL_DEBUG_MSG( 2, ( "=> write finished" ) );

    /*
     * Set the out_msg pointer to the correct location based on IV length
     */
    if( ssl->minor_ver >= SSL_MINOR_VERSION_2 )
    {
        ssl->out_msg = ssl->out_iv + ssl->transform_negotiate->ivlen -
                       ssl->transform_negotiate->fixed_ivlen;
    }
    else
        ssl->out_msg = ssl->out_iv;

    ssl->handshake->calc_finished( ssl, ssl->out_msg + 4, ssl->endpoint );

    // TODO TLS/1.2 Hash length is determined by cipher suite (Page 63)
    hash_len = ( ssl->minor_ver == SSL_MINOR_VERSION_0 ) ? 36 : 12;

#if defined(POLARSSL_SSL_RENEGOTIATION)
    ssl->verify_data_len = hash_len;
    memcpy( ssl->own_verify_data, ssl->out_msg + 4, hash_len );
#endif

    ssl->out_msglen  = 4 + hash_len;
    ssl->out_msgtype = SSL_MSG_HANDSHAKE;
    ssl->out_msg[0]  = SSL_HS_FINISHED;

    /*
     * In case of session resuming, invert the client and server
     * ChangeCipherSpec messages order.
     */
    if( ssl->handshake->resume != 0 )
    {
#if defined(POLARSSL_SSL_CLI_C)
        if( ssl->endpoint == SSL_IS_CLIENT )
            ssl->state = SSL_HANDSHAKE_WRAPUP;
#endif
#if defined(POLARSSL_SSL_SRV_C)
        if( ssl->endpoint == SSL_IS_SERVER )
            ssl->state = SSL_CLIENT_CHANGE_CIPHER_SPEC;
#endif
    }
    else
        ssl->state++;

    /*
     * Switch to our negotiated transform and session parameters for outbound
     * data.
     */
    SSL_DEBUG_MSG( 3, ( "switching to new transform spec for outbound data" ) );
    ssl->transform_out = ssl->transform_negotiate;
    ssl->session_out = ssl->session_negotiate;
    memset( ssl->out_ctr, 0, 8 );

#if defined(POLARSSL_SSL_HW_RECORD_ACCEL)
    if( ssl_hw_record_activate != NULL )
    {
        if( ( ret = ssl_hw_record_activate( ssl, SSL_CHANNEL_OUTBOUND ) ) != 0 )
        {
            SSL_DEBUG_RET( 1, "ssl_hw_record_activate", ret );
            return( POLARSSL_ERR_SSL_HW_ACCEL_FAILED );
        }
    }
#endif

    if( ( ret = ssl_write_record( ssl ) ) != 0 )
    {
        SSL_DEBUG_RET( 1, "ssl_write_record", ret );
        return( ret );
    }

    SSL_DEBUG_MSG( 2, ( "<= write finished" ) );

    return( 0 );
}