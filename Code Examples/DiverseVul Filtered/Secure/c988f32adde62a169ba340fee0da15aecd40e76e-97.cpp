static void ssl_update_checksum_start( ssl_context *ssl,
                                       const unsigned char *buf, size_t len )
{
#if defined(POLARSSL_SSL_PROTO_SSL3) || defined(POLARSSL_SSL_PROTO_TLS1) || \
    defined(POLARSSL_SSL_PROTO_TLS1_1)
     md5_update( &ssl->handshake->fin_md5 , buf, len );
    sha1_update( &ssl->handshake->fin_sha1, buf, len );
#endif
#if defined(POLARSSL_SSL_PROTO_TLS1_2)
#if defined(POLARSSL_SHA256_C)
    sha256_update( &ssl->handshake->fin_sha256, buf, len );
#endif
#if defined(POLARSSL_SHA512_C)
    sha512_update( &ssl->handshake->fin_sha512, buf, len );
#endif
#endif /* POLARSSL_SSL_PROTO_TLS1_2 */
}