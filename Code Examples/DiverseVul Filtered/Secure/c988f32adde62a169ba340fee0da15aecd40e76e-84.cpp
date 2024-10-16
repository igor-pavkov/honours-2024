static int ssl_write_split( ssl_context *ssl,
                            const unsigned char *buf, size_t len )
{
    int ret;

    if( ssl->split_done == SSL_CBC_RECORD_SPLITTING_DISABLED ||
        len <= 1 ||
        ssl->minor_ver > SSL_MINOR_VERSION_1 ||
        cipher_get_cipher_mode( &ssl->transform_out->cipher_ctx_enc )
                                != POLARSSL_MODE_CBC )
    {
        return( ssl_write_real( ssl, buf, len ) );
    }

    if( ssl->split_done == 0 )
    {
        if( ( ret = ssl_write_real( ssl, buf, 1 ) ) <= 0 )
            return( ret );
        ssl->split_done = 1;
    }

    if( ( ret = ssl_write_real( ssl, buf + 1, len - 1 ) ) <= 0 )
        return( ret );
    ssl->split_done = 0;

    return( ret + 1 );
}