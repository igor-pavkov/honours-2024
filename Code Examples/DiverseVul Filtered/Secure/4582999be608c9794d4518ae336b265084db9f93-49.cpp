static size_t ssl_rsa_key_len( void *ctx )
{
    return ( (rsa_context *) ctx )->len;
}