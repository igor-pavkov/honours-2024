void SSL_set_connect_state(SSL *s)
{
    s->server = 0;
    s->shutdown = 0;
    s->state = SSL_ST_CONNECT | SSL_ST_BEFORE;
    s->handshake_func = s->method->ssl_connect;
    /* clear the current cipher */
    ssl_clear_cipher_ctx(s);
    ssl_clear_hash_ctx(&s->read_hash);
    ssl_clear_hash_ctx(&s->write_hash);
}