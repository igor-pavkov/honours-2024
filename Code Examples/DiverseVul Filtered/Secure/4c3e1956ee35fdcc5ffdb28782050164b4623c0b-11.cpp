static void lag_pred_line(LagarithContext *l, uint8_t *buf,
                          int width, int stride, int line)
{
    int L, TL;

    if (!line) {
        int i, align_width = (width - 1) & ~31;
        /* Left prediction only for first line */
        L = l->dsp.add_hfyu_left_prediction(buf + 1, buf + 1,
                                            align_width, buf[0]);
        for (i = align_width + 1; i < width; i++)
            buf[i] += buf[i - 1];
    } else {
        /* Left pixel is actually prev_row[width] */
        L = buf[width - stride - 1];

        if (line == 1) {
            /* Second line, left predict first pixel, the rest of the line is median predicted
             * NOTE: In the case of RGB this pixel is top predicted */
            TL = l->avctx->pix_fmt == AV_PIX_FMT_YUV420P ? buf[-stride] : L;
        } else {
            /* Top left is 2 rows back, last pixel */
            TL = buf[width - (2 * stride) - 1];
        }

        add_lag_median_prediction(buf, buf - stride, buf,
                                  width, &L, &TL);
    }
}