static void ssl_update_checksum_start( ssl_context *ssl, unsigned char *buf,
                                       size_t len )
{
     md5_update( &ssl->handshake->fin_md5 , buf, len );
    sha1_update( &ssl->handshake->fin_sha1, buf, len );
    sha2_update( &ssl->handshake->fin_sha2, buf, len );
#if defined(POLARSSL_SHA4_C)
    sha4_update( &ssl->handshake->fin_sha4, buf, len );
#endif
}