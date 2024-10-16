void ssl_set_session( ssl_context *ssl, const ssl_session *session )
{
    memcpy( ssl->session_negotiate, session, sizeof(ssl_session) );
    ssl->handshake->resume = 1;
}