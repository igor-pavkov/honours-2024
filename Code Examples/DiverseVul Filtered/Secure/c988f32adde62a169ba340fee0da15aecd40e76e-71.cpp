static ssl_key_cert *ssl_add_key_cert( ssl_context *ssl )
{
    ssl_key_cert *key_cert, *last;

    key_cert = polarssl_malloc( sizeof(ssl_key_cert) );
    if( key_cert == NULL )
        return( NULL );

    memset( key_cert, 0, sizeof( ssl_key_cert ) );

    /* Append the new key_cert to the (possibly empty) current list */
    if( ssl->key_cert == NULL )
    {
        ssl->key_cert = key_cert;
        if( ssl->handshake != NULL )
            ssl->handshake->key_cert = key_cert;
    }
    else
    {
        last = ssl->key_cert;
        while( last->next != NULL )
            last = last->next;
        last->next = key_cert;
    }

    return( key_cert );
}