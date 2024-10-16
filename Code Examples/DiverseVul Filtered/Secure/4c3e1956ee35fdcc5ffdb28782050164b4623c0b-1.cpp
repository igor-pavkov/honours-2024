static void lag_pred_line_yuy2(LagarithContext *l, uint8_t *buf,
                               int width, int stride, int line,
                               int is_luma)
{
    int L, TL;

    if (!line) {
        int i, align_width;
        if (is_luma) {
            buf++;
            width--;
        }

        align_width = (width - 1) & ~31;
        l->dsp.add_hfyu_left_prediction(buf + 1, buf + 1, align_width, buf[0]);

        for (i = align_width + 1; i < width; i++)
            buf[i] += buf[i - 1];

        return;
    }
    if (line == 1) {
        const int HEAD = is_luma ? 4 : 2;
        int i;

        L  = buf[width - stride - 1];
        TL = buf[HEAD  - stride - 1];
        for (i = 0; i < HEAD; i++) {
            L += buf[i];
            buf[i] = L;
        }
        for (; i < width; i++) {
            L      = mid_pred(L & 0xFF, buf[i - stride], (L + buf[i - stride] - TL) & 0xFF) + buf[i];
            TL     = buf[i - stride];
            buf[i] = L;
        }
    } else {
        TL = buf[width - (2 * stride) - 1];
        L  = buf[width - stride - 1];
        l->dsp.add_hfyu_median_prediction(buf, buf - stride, buf, width,
                                          &L, &TL);
    }
}