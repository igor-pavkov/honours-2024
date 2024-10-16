int ssl_set_session_tickets( ssl_context *ssl, int use_tickets )
{
    ssl->session_tickets = use_tickets;

#if defined(POLARSSL_SSL_CLI_C)
    if( ssl->endpoint == SSL_IS_CLIENT )
        return( 0 );
#endif

    if( use_tickets == SSL_SESSION_TICKETS_DISABLED )
        return( 0 );

    if( ssl->f_rng == NULL )
        return( POLARSSL_ERR_SSL_BAD_INPUT_DATA );

    return( ssl_ticket_keys_init( ssl ) );
}