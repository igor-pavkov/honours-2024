int ssl_set_own_cert( ssl_context *ssl, x509_crt *own_cert,
                       pk_context *pk_key )
{
    ssl_key_cert *key_cert = ssl_add_key_cert( ssl );

    if( key_cert == NULL )
        return( POLARSSL_ERR_SSL_MALLOC_FAILED );

    key_cert->cert = own_cert;
    key_cert->key  = pk_key;

    return( 0 );
}