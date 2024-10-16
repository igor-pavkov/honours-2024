int ssl_set_own_cert_alt( ssl_context *ssl, x509_crt *own_cert,
                          void *rsa_key,
                          rsa_decrypt_func rsa_decrypt,
                          rsa_sign_func rsa_sign,
                          rsa_key_len_func rsa_key_len )
{
    int ret;
    ssl_key_cert *key_cert = ssl_add_key_cert( ssl );

    if( key_cert == NULL )
        return( POLARSSL_ERR_SSL_MALLOC_FAILED );

    key_cert->key = polarssl_malloc( sizeof(pk_context) );
    if( key_cert->key == NULL )
        return( POLARSSL_ERR_SSL_MALLOC_FAILED );

    pk_init( key_cert->key );

    if( ( ret = pk_init_ctx_rsa_alt( key_cert->key, rsa_key,
                                 rsa_decrypt, rsa_sign, rsa_key_len ) ) != 0 )
        return( ret );

    key_cert->cert = own_cert;
    key_cert->key_own_alloc = 1;

    return( 0 );
}