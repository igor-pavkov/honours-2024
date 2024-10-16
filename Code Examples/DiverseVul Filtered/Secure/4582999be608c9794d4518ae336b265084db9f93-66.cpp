void ssl_set_renegotiation( ssl_context *ssl, int renegotiation )
{
    ssl->disable_renegotiation = renegotiation;
}