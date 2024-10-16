void SSL_CTX_set_psk_server_callback(SSL_CTX *ctx,
                                     unsigned int (*cb) (SSL *ssl,
                                                         const char *identity,
                                                         unsigned char *psk,
                                                         unsigned int
                                                         max_psk_len))
{
    ctx->psk_server_callback = cb;
}