static int init_srp(SSL *s, unsigned int context)
{
    OPENSSL_free(s->srp_ctx.login);
    s->srp_ctx.login = NULL;

    return 1;
}