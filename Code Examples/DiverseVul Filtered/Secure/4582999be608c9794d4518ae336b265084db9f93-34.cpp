const char *ssl_get_ciphersuite( const ssl_context *ssl )
{
    return ssl_get_ciphersuite_name( ssl->session->ciphersuite );
}