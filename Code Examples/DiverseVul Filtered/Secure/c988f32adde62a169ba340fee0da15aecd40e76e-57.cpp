int ssl_set_psk( ssl_context *ssl, const unsigned char *psk, size_t psk_len,
                 const unsigned char *psk_identity, size_t psk_identity_len )
{
    if( psk == NULL || psk_identity == NULL )
        return( POLARSSL_ERR_SSL_BAD_INPUT_DATA );

    if( psk_len > POLARSSL_PSK_MAX_LEN )
        return( POLARSSL_ERR_SSL_BAD_INPUT_DATA );

    /* Identity len will be encoded on two bytes */
    if( ( psk_identity_len >> 16 ) != 0 ||
        psk_identity_len > SSL_MAX_CONTENT_LEN )
    {
        return( POLARSSL_ERR_SSL_BAD_INPUT_DATA );
    }

    if( ssl->psk != NULL || ssl->psk_identity != NULL )
    {
        polarssl_free( ssl->psk );
        polarssl_free( ssl->psk_identity );
    }

    if( ( ssl->psk = polarssl_malloc( psk_len ) ) == NULL ||
        ( ssl->psk_identity = polarssl_malloc( psk_identity_len ) ) == NULL )
    {
        polarssl_free( ssl->psk );
        ssl->psk = NULL;
        return( POLARSSL_ERR_SSL_MALLOC_FAILED );
    }

    ssl->psk_len = psk_len;
    ssl->psk_identity_len = psk_identity_len;

    memcpy( ssl->psk, psk, ssl->psk_len );
    memcpy( ssl->psk_identity, psk_identity, ssl->psk_identity_len );

    return( 0 );
}