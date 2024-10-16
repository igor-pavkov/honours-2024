static int ssl_session_copy( ssl_session *dst, const ssl_session *src )
{
    ssl_session_free( dst );
    memcpy( dst, src, sizeof( ssl_session ) );

#if defined(POLARSSL_X509_CRT_PARSE_C)
    if( src->peer_cert != NULL )
    {
        int ret;

        dst->peer_cert = polarssl_malloc( sizeof(x509_crt) );
        if( dst->peer_cert == NULL )
            return( POLARSSL_ERR_SSL_MALLOC_FAILED );

        x509_crt_init( dst->peer_cert );

        if( ( ret = x509_crt_parse_der( dst->peer_cert, src->peer_cert->raw.p,
                                        src->peer_cert->raw.len ) ) != 0 )
        {
            polarssl_free( dst->peer_cert );
            dst->peer_cert = NULL;
            return( ret );
        }
    }
#endif /* POLARSSL_X509_CRT_PARSE_C */

#if defined(POLARSSL_SSL_SESSION_TICKETS)
    if( src->ticket != NULL )
    {
        dst->ticket = polarssl_malloc( src->ticket_len );
        if( dst->ticket == NULL )
            return( POLARSSL_ERR_SSL_MALLOC_FAILED );

        memcpy( dst->ticket, src->ticket, src->ticket_len );
    }
#endif /* POLARSSL_SSL_SESSION_TICKETS */

    return( 0 );
}