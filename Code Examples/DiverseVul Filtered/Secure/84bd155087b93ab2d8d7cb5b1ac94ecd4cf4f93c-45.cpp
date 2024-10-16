static void mask2shift(uint32_t mask, int *right, int *left)
{
    int rshift = 0, lshift = 0;

    if(!mask)
    {
        *right = *left = 0;
        return;
    }

    while(!(mask & 1))
    {
        mask >>= 1;
        rshift++;
    }
    *right = rshift;

    while(mask & 1)
    {
        mask >>= 1;
        lshift++;
    }
    *left = 12 - lshift;
}