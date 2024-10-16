unsigned long OPENSSL_LH_num_items(const OPENSSL_LHASH *lh)
{
    return lh ? lh->num_items : 0;
}