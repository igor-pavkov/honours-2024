void ssl_handshake_free( ssl_handshake_params *handshake )
{
    if( handshake == NULL )
        return;

#if defined(POLARSSL_DHM_C)
    dhm_free( &handshake->dhm_ctx );
#endif
#if defined(POLARSSL_ECDH_C)
    ecdh_free( &handshake->ecdh_ctx );
#endif

#if defined(POLARSSL_ECDH_C) || defined(POLARSSL_ECDSA_C)
    /* explicit void pointer cast for buggy MS compiler */
    polarssl_free( (void *) handshake->curves );
#endif

#if defined(POLARSSL_X509_CRT_PARSE_C) && \
    defined(POLARSSL_SSL_SERVER_NAME_INDICATION)
    /*
     * Free only the linked list wrapper, not the keys themselves
     * since the belong to the SNI callback
     */
    if( handshake->sni_key_cert != NULL )
    {
        ssl_key_cert *cur = handshake->sni_key_cert, *next;

        while( cur != NULL )
        {
            next = cur->next;
            polarssl_free( cur );
            cur = next;
        }
    }
#endif /* POLARSSL_X509_CRT_PARSE_C && POLARSSL_SSL_SERVER_NAME_INDICATION */

    polarssl_zeroize( handshake, sizeof( ssl_handshake_params ) );
}