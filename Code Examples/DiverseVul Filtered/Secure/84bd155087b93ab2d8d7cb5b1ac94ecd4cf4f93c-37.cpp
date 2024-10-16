static inline void rgb2hsv_default(int r, int g, int b,
                                   int *hue, int *sat, int *val)
{
    int min, max, delta;

    min = r; max = r;
    if(min > g) min = g; if(max < g) max = g;
    if(min > b) min = b; if(max < b) max = b;

    delta = max - min; /* 0 - 0xfff */
    *val = max; /* 0 - 0xfff */

    if(delta)
    {
        *sat = 0xfff * delta / max; /* 0 - 0xfff */

        /* Generate *hue between 0 and 0x5fff */
        if( r == max )
            *hue = 0x1000 + 0x1000 * (g - b) / delta;
        else if( g == max )
            *hue = 0x3000 + 0x1000 * (b - r) / delta;
        else
            *hue = 0x5000 + 0x1000 * (r - g) / delta;
    }
    else
    {
        *sat = 0;
        *hue = 0;
    }
}