static int lag_decode_line(LagarithContext *l, lag_rac *rac,
                           uint8_t *dst, int width, int stride,
                           int esc_count)
{
    int i = 0;
    int ret = 0;

    if (!esc_count)
        esc_count = -1;

    /* Output any zeros remaining from the previous run */
handle_zeros:
    if (l->zeros_rem) {
        int count = FFMIN(l->zeros_rem, width - i);
        memset(dst + i, 0, count);
        i += count;
        l->zeros_rem -= count;
    }

    while (i < width) {
        dst[i] = lag_get_rac(rac);
        ret++;

        if (dst[i])
            l->zeros = 0;
        else
            l->zeros++;

        i++;
        if (l->zeros == esc_count) {
            int index = lag_get_rac(rac);
            ret++;

            l->zeros = 0;

            l->zeros_rem = lag_calc_zero_run(index);
            goto handle_zeros;
        }
    }
    return ret;
}