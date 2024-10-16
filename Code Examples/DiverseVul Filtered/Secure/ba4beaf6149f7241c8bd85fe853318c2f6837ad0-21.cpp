static void predictor_decode_stereo_3800(APEContext *ctx, int count)
{
    APEPredictor *p = &ctx->predictor;
    int32_t *decoded0 = ctx->decoded[0];
    int32_t *decoded1 = ctx->decoded[1];
    int start = 4, shift = 10;

    if (ctx->compression_level == COMPRESSION_LEVEL_HIGH) {
        start = 16;
        long_filter_high_3800(decoded0, 16, 9, count);
        long_filter_high_3800(decoded1, 16, 9, count);
    } else if (ctx->compression_level == COMPRESSION_LEVEL_EXTRA_HIGH) {
        int order = 128, shift2 = 11;

        if (ctx->fileversion >= 3830) {
            order <<= 1;
            shift++;
            shift2++;
            long_filter_ehigh_3830(decoded0 + order, count - order);
            long_filter_ehigh_3830(decoded1 + order, count - order);
        }
        start = order;
        long_filter_high_3800(decoded0, order, shift2, count);
        long_filter_high_3800(decoded1, order, shift2, count);
    }

    while (count--) {
        int X = *decoded0, Y = *decoded1;
        if (ctx->compression_level == COMPRESSION_LEVEL_FAST) {
            *decoded0 = filter_fast_3320(p, Y, 0, YDELAYA);
            decoded0++;
            *decoded1 = filter_fast_3320(p, X, 1, XDELAYA);
            decoded1++;
        } else {
            *decoded0 = filter_3800(p, Y, 0, YDELAYA, YDELAYB,
                                    start, shift);
            decoded0++;
            *decoded1 = filter_3800(p, X, 1, XDELAYA, XDELAYB,
                                    start, shift);
            decoded1++;
        }

        /* Combined */
        p->buf++;
        p->sample_pos++;

        /* Have we filled the history buffer? */
        if (p->buf == p->historybuffer + HISTORY_SIZE) {
            memmove(p->historybuffer, p->buf,
                    PREDICTOR_SIZE * sizeof(*p->historybuffer));
            p->buf = p->historybuffer;
        }
    }
}