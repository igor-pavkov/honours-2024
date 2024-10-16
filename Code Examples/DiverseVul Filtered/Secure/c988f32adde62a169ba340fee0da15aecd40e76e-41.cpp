void ssl_set_renegotiation_enforced( ssl_context *ssl, int max_records )
{
    ssl->renego_max_records = max_records;
}