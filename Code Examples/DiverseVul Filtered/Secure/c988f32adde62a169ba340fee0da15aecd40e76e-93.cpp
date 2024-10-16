static void ssl_key_cert_free( ssl_key_cert *key_cert )
{
    ssl_key_cert *cur = key_cert, *next;

    while( cur != NULL )
    {
        next = cur->next;

        if( cur->key_own_alloc )
        {
            pk_free( cur->key );
            polarssl_free( cur->key );
        }
        polarssl_free( cur );

        cur = next;
    }
}