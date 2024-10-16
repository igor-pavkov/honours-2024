static void restore_rgb_planes(uint8_t *src, int step, int stride, int width,
                               int height)
{
    int i, j;
    uint8_t r, g, b;

    for (j = 0; j < height; j++) {
        for (i = 0; i < width * step; i += step) {
            r = src[i];
            g = src[i + 1];
            b = src[i + 2];
            src[i]     = r + g - 0x80;
            src[i + 2] = b + g - 0x80;
        }
        src += stride;
    }
}