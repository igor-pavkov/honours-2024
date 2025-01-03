static av_always_inline void paint_raw(uint8_t *dst, int w, int h,
                                       GetByteContext *gb, int bpp,
                                       int be, int stride)
{
    int i, j, p;
    for (j = 0; j < h; j++) {
        for (i = 0; i < w; i++) {
            p = vmnc_get_pixel(gb, bpp, be);
            switch (bpp) {
            case 1:
                dst[i] = p;
                break;
            case 2:
                ((uint16_t*)dst)[i] = p;
                break;
            case 4:
                ((uint32_t*)dst)[i] = p;
                break;
            }
        }
        dst += stride;
    }
}