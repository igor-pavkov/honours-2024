static void restore_rgb_planes10(AVFrame *frame, int width, int height)
{
    uint16_t *src_r = (uint16_t *)frame->data[2];
    uint16_t *src_g = (uint16_t *)frame->data[0];
    uint16_t *src_b = (uint16_t *)frame->data[1];
    int r, g, b;
    int i, j;

    for (j = 0; j < height; j++) {
        for (i = 0; i < width; i++) {
            r = src_r[i];
            g = src_g[i];
            b = src_b[i];
            src_r[i] = (r + g - 0x200) & 0x3FF;
            src_b[i] = (b + g - 0x200) & 0x3FF;
        }
        src_r += frame->linesize[2] / 2;
        src_g += frame->linesize[0] / 2;
        src_b += frame->linesize[1] / 2;
    }
}