static int ssl_check_ctr_renegotiate( ssl_context *ssl )
{
    if( ssl->state != SSL_HANDSHAKE_OVER ||
        ssl->renegotiation == SSL_RENEGOTIATION_PENDING ||
        ssl->disable_renegotiation == SSL_RENEGOTIATION_DISABLED )
    {
        return( 0 );
    }

    // TODO: adapt for DTLS
    if( memcmp( ssl->in_ctr,  ssl->renego_period, 8 ) <= 0 &&
        memcmp( ssl->out_ctr, ssl->renego_period, 8 ) <= 0 )
    {
        return( 0 );
    }

    SSL_DEBUG_MSG( 0, ( "record counter limit reached: renegotiate" ) );
    return( ssl_renegotiate( ssl ) );
}