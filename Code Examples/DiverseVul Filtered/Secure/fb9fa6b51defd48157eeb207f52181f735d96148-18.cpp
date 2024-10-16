static int init_ems(SSL *s, unsigned int context)
{
    if (s->s3->flags & TLS1_FLAGS_RECEIVED_EXTMS) {
        s->s3->flags &= ~TLS1_FLAGS_RECEIVED_EXTMS;
        s->s3->flags |= TLS1_FLAGS_REQUIRED_EXTMS;
    }

    return 1;
}