static int asn1_item_flags_i2d(const ASN1_VALUE *val, unsigned char **out,
                               const ASN1_ITEM *it, int flags)
{
    if (out != NULL && *out == NULL) {
        unsigned char *p, *buf;
        int len;

        len = ASN1_item_ex_i2d(&val, NULL, it, -1, flags);
        if (len <= 0)
            return len;
        if ((buf = OPENSSL_malloc(len)) == NULL) {
            ERR_raise(ERR_LIB_ASN1, ERR_R_MALLOC_FAILURE);
            return -1;
        }
        p = buf;
        ASN1_item_ex_i2d(&val, &p, it, -1, flags);
        *out = buf;
        return len;
    }

    return ASN1_item_ex_i2d(&val, out, it, -1, flags);
}