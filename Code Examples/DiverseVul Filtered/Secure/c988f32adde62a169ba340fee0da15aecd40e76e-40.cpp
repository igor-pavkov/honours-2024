static void ssl_handshake_params_init( ssl_handshake_params *handshake )
{
    memset( handshake, 0, sizeof( ssl_handshake_params ) );

#if defined(POLARSSL_SSL_PROTO_SSL3) || defined(POLARSSL_SSL_PROTO_TLS1) || \
    defined(POLARSSL_SSL_PROTO_TLS1_1)
     md5_init(   &handshake->fin_md5  );
    sha1_init(   &handshake->fin_sha1 );
     md5_starts( &handshake->fin_md5  );
    sha1_starts( &handshake->fin_sha1 );
#endif
#if defined(POLARSSL_SSL_PROTO_TLS1_2)
#if defined(POLARSSL_SHA256_C)
    sha256_init(   &handshake->fin_sha256    );
    sha256_starts( &handshake->fin_sha256, 0 );
#endif
#if defined(POLARSSL_SHA512_C)
    sha512_init(   &handshake->fin_sha512    );
    sha512_starts( &handshake->fin_sha512, 1 );
#endif
#endif /* POLARSSL_SSL_PROTO_TLS1_2 */

    handshake->update_checksum = ssl_update_checksum_start;
    handshake->sig_alg = SSL_HASH_SHA1;

#if defined(POLARSSL_DHM_C)
    dhm_init( &handshake->dhm_ctx );
#endif
#if defined(POLARSSL_ECDH_C)
    ecdh_init( &handshake->ecdh_ctx );
#endif
}