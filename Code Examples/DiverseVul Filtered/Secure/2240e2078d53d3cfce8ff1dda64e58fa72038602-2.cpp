static int make_cdt15_entry(int p1, int p2, int16_t *cdt)
{
    int r, b, lo;

    b = cdt[p2];
    r = cdt[p1] << 10;
    lo = b + r;
    return (lo + (lo << 16)) << 1;
}