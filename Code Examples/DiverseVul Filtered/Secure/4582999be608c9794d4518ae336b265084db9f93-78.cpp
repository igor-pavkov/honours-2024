int ssl_session_reset( ssl_context *ssl )
{
    int ret;

    ssl->state = SSL_HELLO_REQUEST;
    ssl->renegotiation = SSL_INITIAL_HANDSHAKE;
    ssl->secure_renegotiation = SSL_LEGACY_RENEGOTIATION;

    ssl->verify_data_len = 0;
    memset( ssl->own_verify_data, 0, 36 );
    memset( ssl->peer_verify_data, 0, 36 );

    ssl->in_offt = NULL;

    ssl->in_msgtype = 0;
    ssl->in_msglen = 0;
    ssl->in_left = 0;

    ssl->in_hslen = 0;
    ssl->nb_zero = 0;

    ssl->out_msgtype = 0;
    ssl->out_msglen = 0;
    ssl->out_left = 0;

    ssl->transform_in = NULL;
    ssl->transform_out = NULL;

    memset( ssl->out_ctr, 0, SSL_BUFFER_LEN );
    memset( ssl->in_ctr, 0, SSL_BUFFER_LEN );

#if defined(POLARSSL_SSL_HW_RECORD_ACCEL)
    if( ssl_hw_record_reset != NULL)
    {
        SSL_DEBUG_MSG( 2, ( "going for ssl_hw_record_reset()" ) );
        if( ssl_hw_record_reset( ssl ) != 0 )
        {
            SSL_DEBUG_RET( 1, "ssl_hw_record_reset", ret );
            return( POLARSSL_ERR_SSL_HW_ACCEL_FAILED );
        }
    }
#endif

    if( ssl->transform )
    {
        ssl_transform_free( ssl->transform );
        free( ssl->transform );
        ssl->transform = NULL;
    }

    if( ( ret = ssl_handshake_init( ssl ) ) != 0 )
        return( ret );

    return( 0 );
}