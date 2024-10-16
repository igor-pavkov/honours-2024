void SSL_set_accept_state(SSL *s)
{
    s->server = 1;
    s->shutdown = 0;
    s->state = SSL_ST_ACCEPT | SSL_ST_BEFORE;
    s->handshake_func = s->method->ssl_accept;
    /* clear the current cipher */
    ssl_clear_cipher_ctx(s);
    ssl_clear_hash_ctx(&s->read_hash);
    ssl_clear_hash_ctx(&s->write_hash);
}