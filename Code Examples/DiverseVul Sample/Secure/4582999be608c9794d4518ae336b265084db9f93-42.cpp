int ssl_flush_output( ssl_context *ssl )
{
    int ret;
    unsigned char *buf;

    SSL_DEBUG_MSG( 2, ( "=> flush output" ) );

    while( ssl->out_left > 0 )
    {
        SSL_DEBUG_MSG( 2, ( "message length: %d, out_left: %d",
                       5 + ssl->out_msglen, ssl->out_left ) );

        if( ssl->out_msglen < ssl->out_left )
        {
            size_t header_left = ssl->out_left - ssl->out_msglen;

            buf = ssl->out_hdr + 5 - header_left;
            ret = ssl->f_send( ssl->p_send, buf, header_left );
            
            SSL_DEBUG_RET( 2, "ssl->f_send (header)", ret );

            if( ret <= 0 )
                return( ret );

            ssl->out_left -= ret;
        }
        
        buf = ssl->out_msg + ssl->out_msglen - ssl->out_left;
        ret = ssl->f_send( ssl->p_send, buf, ssl->out_left );

        SSL_DEBUG_RET( 2, "ssl->f_send", ret );

        if( ret <= 0 )
            return( ret );

        ssl->out_left -= ret;
    }

    SSL_DEBUG_MSG( 2, ( "<= flush output" ) );

    return( 0 );
}