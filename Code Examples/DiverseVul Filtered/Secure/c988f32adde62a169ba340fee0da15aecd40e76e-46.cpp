int ssl_fetch_input( ssl_context *ssl, size_t nb_want )
{
    int ret;
    size_t len;

    SSL_DEBUG_MSG( 2, ( "=> fetch input" ) );

    if( nb_want > SSL_BUFFER_LEN - 8 )
    {
        SSL_DEBUG_MSG( 1, ( "requesting more data than fits" ) );
        return( POLARSSL_ERR_SSL_BAD_INPUT_DATA );
    }

    while( ssl->in_left < nb_want )
    {
        len = nb_want - ssl->in_left;
        ret = ssl->f_recv( ssl->p_recv, ssl->in_hdr + ssl->in_left, len );

        SSL_DEBUG_MSG( 2, ( "in_left: %d, nb_want: %d",
                       ssl->in_left, nb_want ) );
        SSL_DEBUG_RET( 2, "ssl->f_recv", ret );

        if( ret == 0 )
            return( POLARSSL_ERR_SSL_CONN_EOF );

        if( ret < 0 )
            return( ret );

        ssl->in_left += ret;
    }

    SSL_DEBUG_MSG( 2, ( "<= fetch input" ) );

    return( 0 );
}