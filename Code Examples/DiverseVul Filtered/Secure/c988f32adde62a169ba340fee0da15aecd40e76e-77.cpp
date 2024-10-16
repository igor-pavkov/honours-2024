int ssl_get_session( const ssl_context *ssl, ssl_session *dst )
{
    if( ssl == NULL ||
        dst == NULL ||
        ssl->session == NULL ||
        ssl->endpoint != SSL_IS_CLIENT )
    {
        return( POLARSSL_ERR_SSL_BAD_INPUT_DATA );
    }

    return( ssl_session_copy( dst, ssl->session ) );
}