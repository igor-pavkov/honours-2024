void ssl_legacy_renegotiation( ssl_context *ssl, int allow_legacy )
{
    ssl->allow_legacy_renegotiation = allow_legacy;
}