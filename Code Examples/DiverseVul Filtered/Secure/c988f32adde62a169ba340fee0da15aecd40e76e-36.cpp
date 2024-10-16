const char *ssl_get_alpn_protocol( const ssl_context *ssl )
{
    return( ssl->alpn_chosen );
}