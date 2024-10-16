void OPENSSL_LH_set_down_load(OPENSSL_LHASH *lh, unsigned long down_load)
{
    lh->down_load = down_load;
}