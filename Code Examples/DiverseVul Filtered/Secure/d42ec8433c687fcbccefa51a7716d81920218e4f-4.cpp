static void set_palette(uint32_t *pal)
{
    int r, g, b;
    memcpy(pal, ff_cga_palette, 16 * 4);
    pal += 16;
#define COLOR(x) ((x) * 40 + 55)
    for (r = 0; r < 6; r++)
        for (g = 0; g < 6; g++)
            for (b = 0; b < 6; b++)
                *pal++ = 0xFF000000 | (COLOR(r) << 16) | (COLOR(g) << 8) | COLOR(b);
#define GRAY(x) ((x) * 10 + 8)
    for (g = 0; g < 24; g++)
        *pal++ = 0xFF000000 | (GRAY(g) << 16) | (GRAY(g) << 8) | GRAY(g);
}