void ssl_set_session_ticket_lifetime( ssl_context *ssl, int lifetime )
{
    ssl->ticket_lifetime = lifetime;
}