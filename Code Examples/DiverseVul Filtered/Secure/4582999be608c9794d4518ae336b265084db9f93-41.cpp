static int ssl_compress_buf( ssl_context *ssl )
{
    int ret;
    unsigned char *msg_post = ssl->out_msg;
    size_t len_pre = ssl->out_msglen;
    unsigned char *msg_pre;

    SSL_DEBUG_MSG( 2, ( "=> compress buf" ) );

    msg_pre = (unsigned char*) malloc( len_pre );
    if( msg_pre == NULL )
    {
        SSL_DEBUG_MSG( 1, ( "malloc(%d bytes) failed", len_pre ) );
        return( POLARSSL_ERR_SSL_MALLOC_FAILED );
    }

    memcpy( msg_pre, ssl->out_msg, len_pre );

    SSL_DEBUG_MSG( 3, ( "before compression: msglen = %d, ",
                   ssl->out_msglen ) );

    SSL_DEBUG_BUF( 4, "before compression: output payload",
                   ssl->out_msg, ssl->out_msglen );

    ssl->transform_out->ctx_deflate.next_in = msg_pre;
    ssl->transform_out->ctx_deflate.avail_in = len_pre;
    ssl->transform_out->ctx_deflate.next_out = msg_post;
    ssl->transform_out->ctx_deflate.avail_out = SSL_BUFFER_LEN;

    ret = deflate( &ssl->transform_out->ctx_deflate, Z_SYNC_FLUSH );
    if( ret != Z_OK )
    {
        SSL_DEBUG_MSG( 1, ( "failed to perform compression (%d)", ret ) );
        return( POLARSSL_ERR_SSL_COMPRESSION_FAILED );
    }

    ssl->out_msglen = SSL_BUFFER_LEN - ssl->transform_out->ctx_deflate.avail_out;

    free( msg_pre );

    SSL_DEBUG_MSG( 3, ( "after compression: msglen = %d, ",
                   ssl->out_msglen ) );

    SSL_DEBUG_BUF( 4, "after compression: output payload",
                   ssl->out_msg, ssl->out_msglen );

    SSL_DEBUG_MSG( 2, ( "<= compress buf" ) );

    return( 0 );
}