static int init_server_name(SSL *s, unsigned int context)
{
    if (s->server) {
        s->servername_done = 0;

        OPENSSL_free(s->ext.hostname);
        s->ext.hostname = NULL;
    }

    return 1;
}