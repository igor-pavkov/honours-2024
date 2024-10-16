static inline void ok_png_premultiply(uint8_t *dst) {
    const uint8_t a = dst[3];
    if (a == 0) {
        dst[0] = 0;
        dst[1] = 0;
        dst[2] = 0;
    } else if (a < 255) {
        dst[0] = (a * dst[0] + 127) / 255;
        dst[1] = (a * dst[1] + 127) / 255;
        dst[2] = (a * dst[2] + 127) / 255;
    }
}