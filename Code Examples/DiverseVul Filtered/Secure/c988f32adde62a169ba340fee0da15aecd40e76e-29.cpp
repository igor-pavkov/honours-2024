const char *ssl_get_ciphersuite( const ssl_context *ssl )
{
    if( ssl == NULL || ssl->session == NULL )
        return( NULL );

    return ssl_get_ciphersuite_name( ssl->session->ciphersuite );
}