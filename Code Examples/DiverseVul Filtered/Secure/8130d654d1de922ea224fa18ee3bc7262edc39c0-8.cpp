int X509_NAME_cmp(const X509_NAME *a, const X509_NAME *b)
{
    int ret;

    if (b == NULL)
        return a != NULL;
    if (a == NULL)
        return -1;

    /* Ensure canonical encoding is present and up to date */
    if (!a->canon_enc || a->modified) {
        ret = i2d_X509_NAME((X509_NAME *)a, NULL);
        if (ret < 0)
            return -2;
    }

    if (!b->canon_enc || b->modified) {
        ret = i2d_X509_NAME((X509_NAME *)b, NULL);
        if (ret < 0)
            return -2;
    }

    ret = a->canon_enclen - b->canon_enclen;
    if (ret == 0 && a->canon_enclen != 0)
        ret = memcmp(a->canon_enc, b->canon_enc, a->canon_enclen);

    return ret < 0 ? -1 : ret > 0;
}