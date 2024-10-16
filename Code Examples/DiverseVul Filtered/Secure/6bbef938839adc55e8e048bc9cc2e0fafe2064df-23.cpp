void ff_mpeg4_stuffing(PutBitContext *pbc)
{
    int length;
    put_bits(pbc, 1, 0);
    length = (-put_bits_count(pbc)) & 7;
    if (length)
        put_bits(pbc, length, (1 << length) - 1);
}