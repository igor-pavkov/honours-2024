static void FUNC(pred_planar_2)(uint8_t *_src, const uint8_t *_top,
                                const uint8_t *_left, ptrdiff_t stride)
{
    int x, y;
    pixel *src        = (pixel *)_src;
    const pixel *top  = (const pixel *)_top;
    const pixel *left = (const pixel *)_left;
    for (y = 0; y < 16; y++)
        for (x = 0; x < 16; x++)
            POS(x, y) = ((15 - x) * left[y] + (x + 1) * top[16]  +
                         (15 - y) * top[x]  + (y + 1) * left[16] + 16) >> 5;
}