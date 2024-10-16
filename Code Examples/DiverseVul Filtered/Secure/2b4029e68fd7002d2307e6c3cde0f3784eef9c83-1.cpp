int MDC2_Final(unsigned char *md, MDC2_CTX *c)
{
    unsigned int i;
    int j;

    i = c->num;
    j = c->pad_type;
    if ((i > 0) || (j == 2)) {
        if (j == 2)
            c->data[i++] = 0x80;
        memset(&(c->data[i]), 0, MDC2_BLOCK - i);
        mdc2_body(c, c->data, MDC2_BLOCK);
    }
    memcpy(md, (char *)c->h, MDC2_BLOCK);
    memcpy(&(md[MDC2_BLOCK]), (char *)c->hh, MDC2_BLOCK);
    return 1;
}