static void ssl_update_checksum_sha384( ssl_context *ssl, unsigned char *buf,
                                        size_t len )
{
    sha4_update( &ssl->handshake->fin_sha4, buf, len );
}