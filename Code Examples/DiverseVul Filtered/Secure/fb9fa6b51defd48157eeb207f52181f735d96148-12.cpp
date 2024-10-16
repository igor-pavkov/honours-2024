static int init_npn(SSL *s, unsigned int context)
{
    s->s3->npn_seen = 0;

    return 1;
}