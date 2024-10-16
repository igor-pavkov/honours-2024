void ssl_set_encrypt_then_mac( ssl_context *ssl, char etm )
{
    ssl->encrypt_then_mac = etm;
}