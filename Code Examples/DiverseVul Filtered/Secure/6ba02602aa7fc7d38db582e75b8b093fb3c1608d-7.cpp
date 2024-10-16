static av_always_inline void paint_rect(uint8_t *dst, int dx, int dy,
                                        int w, int h, int color,
                                        int bpp, int stride)
{
    int i, j;
    dst += dx * bpp + dy * stride;
    if (bpp == 1) {
        for (j = 0; j < h; j++) {
            memset(dst, color, w);
            dst += stride;
        }
    } else if (bpp == 2) {
        uint16_t *dst2;
        for (j = 0; j < h; j++) {
            dst2 = (uint16_t*)dst;
            for (i = 0; i < w; i++)
                *dst2++ = color;
            dst += stride;
        }
    } else if (bpp == 4) {
        uint32_t *dst2;
        for (j = 0; j < h; j++) {
            dst2 = (uint32_t*)dst;
            for (i = 0; i < w; i++)
                dst2[i] = color;
            dst += stride;
        }
    }
}