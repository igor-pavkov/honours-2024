int ssl_set_truncated_hmac( ssl_context *ssl, int truncate )
{
    ssl->trunc_hmac = truncate;

    return( 0 );
}