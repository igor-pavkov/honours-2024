void ssl_set_endpoint( ssl_context *ssl, int endpoint )
{
    ssl->endpoint   = endpoint;

#if defined(POLARSSL_SSL_SESSION_TICKETS) && \
    defined(POLARSSL_SSL_CLI_C)
    if( endpoint == SSL_IS_CLIENT )
        ssl->session_tickets = SSL_SESSION_TICKETS_ENABLED;
#endif

#if defined(POLARSSL_SSL_TRUNCATED_HMAC)
    if( endpoint == SSL_IS_SERVER )
        ssl->trunc_hmac = SSL_TRUNC_HMAC_ENABLED;
#endif
}