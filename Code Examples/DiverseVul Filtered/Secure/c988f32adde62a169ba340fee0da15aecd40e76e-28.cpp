int ssl_set_own_cert_rsa( ssl_context *ssl, x509_crt *own_cert,
                           rsa_context *rsa_key )
{
    int ret;
    ssl_key_cert *key_cert = ssl_add_key_cert( ssl );

    if( key_cert == NULL )
        return( POLARSSL_ERR_SSL_MALLOC_FAILED );

    key_cert->key = polarssl_malloc( sizeof(pk_context) );
    if( key_cert->key == NULL )
        return( POLARSSL_ERR_SSL_MALLOC_FAILED );

    pk_init( key_cert->key );

    ret = pk_init_ctx( key_cert->key, pk_info_from_type( POLARSSL_PK_RSA ) );
    if( ret != 0 )
        return( ret );

    if( ( ret = rsa_copy( pk_rsa( *key_cert->key ), rsa_key ) ) != 0 )
        return( ret );

    key_cert->cert = own_cert;
    key_cert->key_own_alloc = 1;

    return( 0 );
}