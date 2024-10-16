int ssl_set_alpn_protocols( ssl_context *ssl, const char **protos )
{
    size_t cur_len, tot_len;
    const char **p;

    /*
     * "Empty strings MUST NOT be included and byte strings MUST NOT be
     * truncated". Check lengths now rather than later.
     */
    tot_len = 0;
    for( p = protos; *p != NULL; p++ )
    {
        cur_len = strlen( *p );
        tot_len += cur_len;

        if( cur_len == 0 || cur_len > 255 || tot_len > 65535 )
            return( POLARSSL_ERR_SSL_BAD_INPUT_DATA );
    }

    ssl->alpn_list = protos;

    return( 0 );
}