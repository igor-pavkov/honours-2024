int ssl_get_verify_result( const ssl_context *ssl )
{
    return( ssl->verify_result );
}