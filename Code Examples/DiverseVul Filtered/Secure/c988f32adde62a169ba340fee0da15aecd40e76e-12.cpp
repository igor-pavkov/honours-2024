int ssl_set_max_frag_len( ssl_context *ssl, unsigned char mfl_code )
{
    if( mfl_code >= SSL_MAX_FRAG_LEN_INVALID ||
        mfl_code_to_length[mfl_code] > SSL_MAX_CONTENT_LEN )
    {
        return( POLARSSL_ERR_SSL_BAD_INPUT_DATA );
    }

    ssl->mfl_code = mfl_code;

    return( 0 );
}