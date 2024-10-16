int ssl3_do_uncompress(SSL *ssl)
{
#ifndef OPENSSL_NO_COMP
    int i;
    SSL3_RECORD *rr;

    rr = &(ssl->s3->rrec);
    i = COMP_expand_block(ssl->expand, rr->comp,
                          SSL3_RT_MAX_PLAIN_LENGTH, rr->data,
                          (int)rr->length);
    if (i < 0)
        return (0);
    else
        rr->length = i;
    rr->data = rr->comp;
#endif
    return (1);
}