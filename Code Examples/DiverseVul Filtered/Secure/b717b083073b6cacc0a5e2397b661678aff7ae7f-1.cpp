static int collect_data(BUF_MEM *buf, const unsigned char **p, long plen)
{
    int len;
    if (buf) {
        len = buf->length;
        if (!BUF_MEM_grow_clean(buf, len + plen)) {
            ASN1err(ASN1_F_COLLECT_DATA, ERR_R_MALLOC_FAILURE);
            return 0;
        }
        memcpy(buf->data + len, *p, plen);
    }
    *p += plen;
    return 1;
}