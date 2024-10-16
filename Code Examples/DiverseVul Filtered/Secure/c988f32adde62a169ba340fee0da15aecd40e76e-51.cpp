static void ssl_update_checksum_md5sha1( ssl_context *ssl,
                                         const unsigned char *buf, size_t len )
{
     md5_update( &ssl->handshake->fin_md5 , buf, len );
    sha1_update( &ssl->handshake->fin_sha1, buf, len );
}