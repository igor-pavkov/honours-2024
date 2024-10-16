void ssl_session_init( ssl_session *session )
{
    memset( session, 0, sizeof(ssl_session) );
}