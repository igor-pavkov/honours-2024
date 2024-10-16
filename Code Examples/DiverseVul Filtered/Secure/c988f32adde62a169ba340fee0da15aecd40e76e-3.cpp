pk_type_t ssl_pk_alg_from_sig( unsigned char sig )
{
    switch( sig )
    {
#if defined(POLARSSL_RSA_C)
        case SSL_SIG_RSA:
            return( POLARSSL_PK_RSA );
#endif
#if defined(POLARSSL_ECDSA_C)
        case SSL_SIG_ECDSA:
            return( POLARSSL_PK_ECDSA );
#endif
        default:
            return( POLARSSL_PK_NONE );
    }
}