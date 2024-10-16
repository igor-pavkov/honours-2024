static void ssl_update_checksum_sha384( ssl_context *ssl,
                                        const unsigned char *buf, size_t len )
{
    sha512_update( &ssl->handshake->fin_sha512, buf, len );
}