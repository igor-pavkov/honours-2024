void OPENSSL_LH_free(OPENSSL_LHASH *lh)
{
    if (lh == NULL)
        return;

    OPENSSL_LH_flush(lh);
    OPENSSL_free(lh->b);
    OPENSSL_free(lh);
}