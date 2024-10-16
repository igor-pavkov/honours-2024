static int ssl_ticket_keys_init( ssl_context *ssl )
{
    int ret;
    ssl_ticket_keys *tkeys;
    unsigned char buf[16];

    if( ssl->ticket_keys != NULL )
        return( 0 );

    tkeys = polarssl_malloc( sizeof(ssl_ticket_keys) );
    if( tkeys == NULL )
        return( POLARSSL_ERR_SSL_MALLOC_FAILED );

    aes_init( &tkeys->enc );
    aes_init( &tkeys->dec );

    if( ( ret = ssl->f_rng( ssl->p_rng, tkeys->key_name, 16 ) ) != 0 )
    {
        ssl_ticket_keys_free( tkeys );
        polarssl_free( tkeys );
        return( ret );
    }

    if( ( ret = ssl->f_rng( ssl->p_rng, buf, 16 ) ) != 0 ||
        ( ret = aes_setkey_enc( &tkeys->enc, buf, 128 ) ) != 0 ||
        ( ret = aes_setkey_dec( &tkeys->dec, buf, 128 ) ) != 0 )
    {
        ssl_ticket_keys_free( tkeys );
        polarssl_free( tkeys );
        return( ret );
    }

    if( ( ret = ssl->f_rng( ssl->p_rng, tkeys->mac_key, 16 ) ) != 0 )
    {
        ssl_ticket_keys_free( tkeys );
        polarssl_free( tkeys );
        return( ret );
    }

    ssl->ticket_keys = tkeys;

    return( 0 );
}