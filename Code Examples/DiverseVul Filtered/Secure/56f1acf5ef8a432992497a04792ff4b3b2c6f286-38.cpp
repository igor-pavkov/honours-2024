void SSL_set_psk_client_callback(SSL *s,
                                 unsigned int (*cb) (SSL *ssl,
                                                     const char *hint,
                                                     char *identity,
                                                     unsigned int
                                                     max_identity_len,
                                                     unsigned char *psk,
                                                     unsigned int
                                                     max_psk_len))
{
    s->psk_client_callback = cb;
}