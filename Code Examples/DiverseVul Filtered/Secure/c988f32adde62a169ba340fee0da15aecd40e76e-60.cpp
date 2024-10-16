static void ssl_ticket_keys_free( ssl_ticket_keys *tkeys )
{
    aes_free( &tkeys->enc );
    aes_free( &tkeys->dec );

    polarssl_zeroize( tkeys, sizeof(ssl_ticket_keys) );
}