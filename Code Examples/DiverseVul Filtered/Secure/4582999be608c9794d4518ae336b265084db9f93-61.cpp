void ssl_handshake_free( ssl_handshake_params *handshake )
{
#if defined(POLARSSL_DHM_C)
    dhm_free( &handshake->dhm_ctx );
#endif
    memset( handshake, 0, sizeof( ssl_handshake_params ) );
}