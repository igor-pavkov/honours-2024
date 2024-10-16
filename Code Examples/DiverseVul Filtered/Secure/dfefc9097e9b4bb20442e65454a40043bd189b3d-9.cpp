static void FUNC(pred_angular_3)(uint8_t *src, const uint8_t *top,
                                 const uint8_t *left,
                                 ptrdiff_t stride, int c_idx, int mode)
{
    FUNC(pred_angular)(src, top, left, stride, c_idx, mode, 1 << 5);
}