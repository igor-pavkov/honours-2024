void *OPENSSL_LH_retrieve(OPENSSL_LHASH *lh, const void *data)
{
    unsigned long hash;
    OPENSSL_LH_NODE **rn;

    if (lh->error != 0)
        lh->error = 0;

    rn = getrn(lh, data, &hash);

    return *rn == NULL ? NULL : (*rn)->data;
}