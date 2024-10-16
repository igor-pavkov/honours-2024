static void FUNC(pred_planar_3)(uint8_t *_src, const uint8_t *_top,
                                const uint8_t *_left, ptrdiff_t stride)
{
    int x, y;
    pixel *src        = (pixel *)_src;
    const pixel *top  = (const pixel *)_top;
    const pixel *left = (const pixel *)_left;
    for (y = 0; y < 32; y++)
        for (x = 0; x < 32; x++)
            POS(x, y) = ((31 - x) * left[y] + (x + 1) * top[32]  +
                         (31 - y) * top[x]  + (y + 1) * left[32] + 32) >> 6;
}