unsigned char ssl_sig_from_pk( pk_context *pk )
{
#if defined(POLARSSL_RSA_C)
    if( pk_can_do( pk, POLARSSL_PK_RSA ) )
        return( SSL_SIG_RSA );
#endif
#if defined(POLARSSL_ECDSA_C)
    if( pk_can_do( pk, POLARSSL_PK_ECDSA ) )
        return( SSL_SIG_ECDSA );
#endif
    return( SSL_SIG_ANON );
}