void ssl_session_free( ssl_session *session )
{
    if( session == NULL )
        return;

#if defined(POLARSSL_X509_CRT_PARSE_C)
    if( session->peer_cert != NULL )
    {
        x509_crt_free( session->peer_cert );
        polarssl_free( session->peer_cert );
    }
#endif

#if defined(POLARSSL_SSL_SESSION_TICKETS)
    polarssl_free( session->ticket );
#endif

    polarssl_zeroize( session, sizeof( ssl_session ) );
}