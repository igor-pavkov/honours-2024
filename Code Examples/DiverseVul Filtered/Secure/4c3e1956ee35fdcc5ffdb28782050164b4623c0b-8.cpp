static void add_lag_median_prediction(uint8_t *dst, uint8_t *src1,
                                      uint8_t *diff, int w, int *left,
                                      int *left_top)
{
    /* This is almost identical to add_hfyu_median_prediction in dsputil.h.
     * However the &0xFF on the gradient predictor yealds incorrect output
     * for lagarith.
     */
    int i;
    uint8_t l, lt;

    l  = *left;
    lt = *left_top;

    for (i = 0; i < w; i++) {
        l = mid_pred(l, src1[i], l + src1[i] - lt) + diff[i];
        lt = src1[i];
        dst[i] = l;
    }

    *left     = l;
    *left_top = lt;
}