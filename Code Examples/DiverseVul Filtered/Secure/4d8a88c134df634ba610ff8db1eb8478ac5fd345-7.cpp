static ossl_inline void set_bit(BN_ULONG *a, int idx)
{
    assert(a != NULL);

    {
        int i, j;

        i = idx / BN_BITS2;
        j = idx % BN_BITS2;
        a[i] |= (((BN_ULONG)1) << j);
    }
}