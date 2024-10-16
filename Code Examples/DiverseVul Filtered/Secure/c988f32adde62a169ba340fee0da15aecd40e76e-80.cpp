void ssl_set_cbc_record_splitting( ssl_context *ssl, char split )
{
    ssl->split_done = split;
}