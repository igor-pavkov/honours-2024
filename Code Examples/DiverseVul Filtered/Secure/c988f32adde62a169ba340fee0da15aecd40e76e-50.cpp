static void ssl_update_checksum_sha256( ssl_context *ssl,
                                        const unsigned char *buf, size_t len )
{
    sha256_update( &ssl->handshake->fin_sha256, buf, len );
}