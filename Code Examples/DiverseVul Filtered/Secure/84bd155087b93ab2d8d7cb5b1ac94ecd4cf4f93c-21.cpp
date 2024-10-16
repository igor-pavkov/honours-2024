static int init_lookup(void)
{
    int v, s, h;

    /* These ones are constant */
    lookup_colors[0] = CACA_BLACK;
    lookup_colors[1] = CACA_DARKGRAY;
    lookup_colors[2] = CACA_LIGHTGRAY;
    lookup_colors[3] = CACA_WHITE;

    /* These ones will be overwritten */
    lookup_colors[4] = CACA_MAGENTA;
    lookup_colors[5] = CACA_LIGHTMAGENTA;
    lookup_colors[6] = CACA_RED;
    lookup_colors[7] = CACA_LIGHTRED;

    for(v = 0; v < LOOKUP_VAL; v++)
        for(s = 0; s < LOOKUP_SAT; s++)
            for(h = 0; h < LOOKUP_HUE; h++)
    {
        int i, distbg, distfg, dist;
        int val, sat, hue;
        uint8_t outbg, outfg;

        val = 0xfff * v / (LOOKUP_VAL - 1);
        sat = 0xfff * s / (LOOKUP_SAT - 1);
        hue = 0xfff * h / (LOOKUP_HUE - 1);

        /* Initialise distances to the distance between pure black HSV
         * coordinates and our white colour (3) */
        outbg = outfg = 3;
        distbg = distfg = HSV_DISTANCE(0, 0, 0, 3);

        /* Calculate distances to eight major colour values and store the
         * two nearest points in our lookup table. */
        for(i = 0; i < 8; i++)
        {
            dist = HSV_DISTANCE(hue, sat, val, i);
            if(dist <= distbg)
            {
                outfg = outbg;
                distfg = distbg;
                outbg = i;
                distbg = dist;
            }
            else if(dist <= distfg)
            {
                outfg = i;
                distfg = dist;
            }
        }

        hsv_distances[v][s][h] = (outfg << 4) | outbg;
    }

    return 0;
}