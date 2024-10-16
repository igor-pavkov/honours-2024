void ssl_session_free( ssl_session *session )
{
    if( session->peer_cert != NULL )
    {
        x509_free( session->peer_cert );
        free( session->peer_cert );
    }

    memset( session, 0, sizeof( ssl_session ) );
}