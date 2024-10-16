static int init_alpn(SSL *s, unsigned int context)
{
    OPENSSL_free(s->s3->alpn_selected);
    s->s3->alpn_selected = NULL;
    s->s3->alpn_selected_len = 0;
    if (s->server) {
        OPENSSL_free(s->s3->alpn_proposed);
        s->s3->alpn_proposed = NULL;
        s->s3->alpn_proposed_len = 0;
    }
    return 1;
}