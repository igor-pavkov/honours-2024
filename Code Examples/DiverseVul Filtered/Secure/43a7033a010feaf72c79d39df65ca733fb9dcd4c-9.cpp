static int collect_data(BUF_MEM *buf, const unsigned char **p, long plen)
{
    int len;
    if (buf) {
        len = buf->length;
        if (!BUF_MEM_grow_clean(buf, len + plen)) {
            ERR_raise(ERR_LIB_ASN1, ERR_R_MALLOC_FAILURE);
            return 0;
        }
        memcpy(buf->data + len, *p, plen);
    }
    *p += plen;
    return 1;
}