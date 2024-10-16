static av_always_inline void FUNC(pred_angular)(uint8_t *_src,
                                                const uint8_t *_top,
                                                const uint8_t *_left,
                                                ptrdiff_t stride, int c_idx,
                                                int mode, int size)
{
    int x, y;
    pixel *src        = (pixel *)_src;
    const pixel *top  = (const pixel *)_top;
    const pixel *left = (const pixel *)_left;

    static const int intra_pred_angle[] = {
         32,  26,  21,  17, 13,  9,  5, 2, 0, -2, -5, -9, -13, -17, -21, -26, -32,
        -26, -21, -17, -13, -9, -5, -2, 0, 2,  5,  9, 13,  17,  21,  26,  32
    };
    static const int inv_angle[] = {
        -4096, -1638, -910, -630, -482, -390, -315, -256, -315, -390, -482,
        -630, -910, -1638, -4096
    };

    int angle = intra_pred_angle[mode - 2];
    pixel ref_array[3 * MAX_TB_SIZE + 1];
    pixel *ref_tmp = ref_array + size;
    const pixel *ref;
    int last = (size * angle) >> 5;

    if (mode >= 18) {
        ref = top - 1;
        if (angle < 0 && last < -1) {
            for (x = 0; x <= size; x++)
                ref_tmp[x] = top[x - 1];
            for (x = last; x <= -1; x++)
                ref_tmp[x] = left[-1 + ((x * inv_angle[mode - 11] + 128) >> 8)];
            ref = ref_tmp;
        }

        for (y = 0; y < size; y++) {
            int idx  = ((y + 1) * angle) >> 5;
            int fact = ((y + 1) * angle) & 31;
            if (fact) {
                for (x = 0; x < size; x++) {
                    POS(x, y) = ((32 - fact) * ref[x + idx + 1] +
                                       fact  * ref[x + idx + 2] + 16) >> 5;
                }
            } else {
                for (x = 0; x < size; x++)
                    POS(x, y) = ref[x + idx + 1];
            }
        }
        if (mode == 26 && c_idx == 0 && size < 32) {
            for (y = 0; y < size; y++)
                POS(0, y) = av_clip_pixel(top[0] + ((left[y] - left[-1]) >> 1));
        }
    } else {
        ref = left - 1;
        if (angle < 0 && last < -1) {
            for (x = 0; x <= size; x++)
                ref_tmp[x] = left[x - 1];
            for (x = last; x <= -1; x++)
                ref_tmp[x] = top[-1 + ((x * inv_angle[mode - 11] + 128) >> 8)];
            ref = ref_tmp;
        }

        for (x = 0; x < size; x++) {
            int idx  = ((x + 1) * angle) >> 5;
            int fact = ((x + 1) * angle) & 31;
            if (fact) {
                for (y = 0; y < size; y++) {
                    POS(x, y) = ((32 - fact) * ref[y + idx + 1] +
                                       fact  * ref[y + idx + 2] + 16) >> 5;
                }
            } else {
                for (y = 0; y < size; y++)
                    POS(x, y) = ref[y + idx + 1];
            }
        }
        if (mode == 10 && c_idx == 0 && size < 32) {
            for (x = 0; x < size; x++)
                POS(x, 0) = av_clip_pixel(left[0] + ((top[x] - top[-1]) >> 1));
        }
    }
}