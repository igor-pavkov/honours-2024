void ssl_set_renegotiation_period( ssl_context *ssl,
                                   const unsigned char period[8] )
{
    memcpy( ssl->renego_period, period, 8 );
}