static int init_post_handshake_auth(SSL *s, unsigned int context)
{
    s->post_handshake_auth = SSL_PHA_NONE;

    return 1;
}