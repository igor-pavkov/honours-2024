static int make_ydt15_entry(int p1, int p2, int16_t *ydt)
#endif
{
    int lo, hi;

    lo = ydt[p1];
    lo += (lo << 5) + (lo << 10);
    hi = ydt[p2];
    hi += (hi << 5) + (hi << 10);
    return (lo + (hi << 16)) << 1;
}