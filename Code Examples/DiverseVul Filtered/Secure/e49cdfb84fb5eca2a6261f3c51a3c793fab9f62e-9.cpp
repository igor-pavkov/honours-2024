static inline void ok_png_unpremultiply(uint8_t *dst) {
    const uint8_t a = dst[3];
    if (a > 0 && a < 255) {
        dst[0] = 255 * dst[0] / a;
        dst[1] = 255 * dst[1] / a;
        dst[2] = 255 * dst[2] / a;
    }
}