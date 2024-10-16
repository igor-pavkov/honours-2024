static int make_ydt16_entry(int p1, int p2, int16_t *ydt)
#endif
{
    int lo, hi;

    lo = ydt[p1];
    lo += (lo << 6) + (lo << 11);
    hi = ydt[p2];
    hi += (hi << 6) + (hi << 11);
    return (lo + (hi << 16)) << 1;
}