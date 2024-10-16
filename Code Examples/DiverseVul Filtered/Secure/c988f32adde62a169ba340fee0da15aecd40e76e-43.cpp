void ssl_set_arc4_support( ssl_context *ssl, char arc4 )
{
    ssl->arc4_disabled = arc4;
}