static int init_srtp(SSL *s, unsigned int context)
{
    if (s->server)
        s->srtp_profile = NULL;

    return 1;
}