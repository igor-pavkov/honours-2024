void ssl_set_extended_master_secret( ssl_context *ssl, char ems )
{
    ssl->extended_ms = ems;
}