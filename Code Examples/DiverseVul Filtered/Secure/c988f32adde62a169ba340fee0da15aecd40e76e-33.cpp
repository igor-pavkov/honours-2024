int ssl_write_record( ssl_context *ssl )
{
    int ret, done = 0;
    size_t len = ssl->out_msglen;

    SSL_DEBUG_MSG( 2, ( "=> write record" ) );

    if( ssl->out_msgtype == SSL_MSG_HANDSHAKE )
    {
        ssl->out_msg[1] = (unsigned char)( ( len - 4 ) >> 16 );
        ssl->out_msg[2] = (unsigned char)( ( len - 4 ) >>  8 );
        ssl->out_msg[3] = (unsigned char)( ( len - 4 )       );

        if( ssl->out_msg[0] != SSL_HS_HELLO_REQUEST )
            ssl->handshake->update_checksum( ssl, ssl->out_msg, len );
    }

#if defined(POLARSSL_ZLIB_SUPPORT)
    if( ssl->transform_out != NULL &&
        ssl->session_out->compression == SSL_COMPRESS_DEFLATE )
    {
        if( ( ret = ssl_compress_buf( ssl ) ) != 0 )
        {
            SSL_DEBUG_RET( 1, "ssl_compress_buf", ret );
            return( ret );
        }

        len = ssl->out_msglen;
    }
#endif /*POLARSSL_ZLIB_SUPPORT */

#if defined(POLARSSL_SSL_HW_RECORD_ACCEL)
    if( ssl_hw_record_write != NULL )
    {
        SSL_DEBUG_MSG( 2, ( "going for ssl_hw_record_write()" ) );

        ret = ssl_hw_record_write( ssl );
        if( ret != 0 && ret != POLARSSL_ERR_SSL_HW_ACCEL_FALLTHROUGH )
        {
            SSL_DEBUG_RET( 1, "ssl_hw_record_write", ret );
            return( POLARSSL_ERR_SSL_HW_ACCEL_FAILED );
        }

        if( ret == 0 )
            done = 1;
    }
#endif /* POLARSSL_SSL_HW_RECORD_ACCEL */
    if( !done )
    {
        ssl->out_hdr[0] = (unsigned char) ssl->out_msgtype;
        ssl->out_hdr[1] = (unsigned char) ssl->major_ver;
        ssl->out_hdr[2] = (unsigned char) ssl->minor_ver;
        ssl->out_hdr[3] = (unsigned char)( len >> 8 );
        ssl->out_hdr[4] = (unsigned char)( len      );

        if( ssl->transform_out != NULL )
        {
            if( ( ret = ssl_encrypt_buf( ssl ) ) != 0 )
            {
                SSL_DEBUG_RET( 1, "ssl_encrypt_buf", ret );
                return( ret );
            }

            len = ssl->out_msglen;
            ssl->out_hdr[3] = (unsigned char)( len >> 8 );
            ssl->out_hdr[4] = (unsigned char)( len      );
        }

        ssl->out_left = 5 + ssl->out_msglen;

        SSL_DEBUG_MSG( 3, ( "output record: msgtype = %d, "
                            "version = [%d:%d], msglen = %d",
                       ssl->out_hdr[0], ssl->out_hdr[1], ssl->out_hdr[2],
                     ( ssl->out_hdr[3] << 8 ) | ssl->out_hdr[4] ) );

        SSL_DEBUG_BUF( 4, "output record sent to network",
                       ssl->out_hdr, 5 + ssl->out_msglen );
    }

    if( ( ret = ssl_flush_output( ssl ) ) != 0 )
    {
        SSL_DEBUG_RET( 1, "ssl_flush_output", ret );
        return( ret );
    }

    SSL_DEBUG_MSG( 2, ( "<= write record" ) );

    return( 0 );
}