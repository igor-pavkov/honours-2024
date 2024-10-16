static void ssl_update_checksum_sha256( ssl_context *ssl, unsigned char *buf,
                                        size_t len )
{
    sha2_update( &ssl->handshake->fin_sha2, buf, len );
}