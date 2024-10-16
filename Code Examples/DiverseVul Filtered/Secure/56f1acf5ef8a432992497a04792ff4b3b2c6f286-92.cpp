void SSL_set_psk_server_callback(SSL *s,
                                 unsigned int (*cb) (SSL *ssl,
                                                     const char *identity,
                                                     unsigned char *psk,
                                                     unsigned int
                                                     max_psk_len))
{
    s->psk_server_callback = cb;
}