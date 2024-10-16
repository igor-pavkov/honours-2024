int ssl_session_reset( ssl_context *ssl )
{
    int ret;

    ssl->state = SSL_HELLO_REQUEST;

#if defined(POLARSSL_SSL_RENEGOTIATION)
    ssl->renegotiation = SSL_INITIAL_HANDSHAKE;
    ssl->renego_records_seen = 0;

    ssl->verify_data_len = 0;
    memset( ssl->own_verify_data, 0, SSL_VERIFY_DATA_MAX_LEN );
    memset( ssl->peer_verify_data, 0, SSL_VERIFY_DATA_MAX_LEN );
#endif
    ssl->secure_renegotiation = SSL_LEGACY_RENEGOTIATION;

    ssl->in_offt = NULL;

    ssl->in_msg = ssl->in_ctr + 13;
    ssl->in_msgtype = 0;
    ssl->in_msglen = 0;
    ssl->in_left = 0;

    ssl->in_hslen = 0;
    ssl->nb_zero = 0;
    ssl->record_read = 0;

    ssl->out_msg = ssl->out_ctr + 13;
    ssl->out_msgtype = 0;
    ssl->out_msglen = 0;
    ssl->out_left = 0;
#if defined(POLARSSL_SSL_CBC_RECORD_SPLITTING)
    if( ssl->split_done != SSL_CBC_RECORD_SPLITTING_DISABLED )
        ssl->split_done = 0;
#endif

    ssl->transform_in = NULL;
    ssl->transform_out = NULL;

    memset( ssl->out_ctr, 0, SSL_BUFFER_LEN );
    memset( ssl->in_ctr, 0, SSL_BUFFER_LEN );

#if defined(POLARSSL_SSL_HW_RECORD_ACCEL)
    if( ssl_hw_record_reset != NULL )
    {
        SSL_DEBUG_MSG( 2, ( "going for ssl_hw_record_reset()" ) );
        if( ( ret = ssl_hw_record_reset( ssl ) ) != 0 )
        {
            SSL_DEBUG_RET( 1, "ssl_hw_record_reset", ret );
            return( POLARSSL_ERR_SSL_HW_ACCEL_FAILED );
        }
    }
#endif

    if( ssl->transform )
    {
        ssl_transform_free( ssl->transform );
        polarssl_free( ssl->transform );
        ssl->transform = NULL;
    }

    if( ssl->session )
    {
        ssl_session_free( ssl->session );
        polarssl_free( ssl->session );
        ssl->session = NULL;
    }

#if defined(POLARSSL_SSL_ALPN)
    ssl->alpn_chosen = NULL;
#endif

    if( ( ret = ssl_handshake_init( ssl ) ) != 0 )
        return( ret );

    return( 0 );
}