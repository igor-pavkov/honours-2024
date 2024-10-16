int ssl_parse_change_cipher_spec( ssl_context *ssl )
{
    int ret;

    SSL_DEBUG_MSG( 2, ( "=> parse change cipher spec" ) );

    if( ( ret = ssl_read_record( ssl ) ) != 0 )
    {
        SSL_DEBUG_RET( 1, "ssl_read_record", ret );
        return( ret );
    }

    if( ssl->in_msgtype != SSL_MSG_CHANGE_CIPHER_SPEC )
    {
        SSL_DEBUG_MSG( 1, ( "bad change cipher spec message" ) );
        return( POLARSSL_ERR_SSL_UNEXPECTED_MESSAGE );
    }

    if( ssl->in_msglen != 1 || ssl->in_msg[0] != 1 )
    {
        SSL_DEBUG_MSG( 1, ( "bad change cipher spec message" ) );
        return( POLARSSL_ERR_SSL_BAD_HS_CHANGE_CIPHER_SPEC );
    }

    ssl->state++;

    SSL_DEBUG_MSG( 2, ( "<= parse change cipher spec" ) );

    return( 0 );
}