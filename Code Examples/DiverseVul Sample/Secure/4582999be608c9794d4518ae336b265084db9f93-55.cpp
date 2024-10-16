void ssl_optimize_checksum( ssl_context *ssl, int ciphersuite )
{
#if !defined(POLARSSL_SHA4_C)
    ((void) ciphersuite);
#endif

    if( ssl->minor_ver < SSL_MINOR_VERSION_3 )
        ssl->handshake->update_checksum = ssl_update_checksum_md5sha1;
#if defined(POLARSSL_SHA4_C)
    else if ( ciphersuite == TLS_RSA_WITH_AES_256_GCM_SHA384 ||
              ciphersuite == TLS_DHE_RSA_WITH_AES_256_GCM_SHA384 )
    {
        ssl->handshake->update_checksum = ssl_update_checksum_sha384;
    }
#endif
    else
        ssl->handshake->update_checksum = ssl_update_checksum_sha256;
}