static int mpeg4_decode_dpcm_macroblock(MpegEncContext *s, int16_t macroblock[256], int n)
{
    int i, j, w, h, idx = 0;
    int block_mean, rice_parameter, rice_prefix_code, rice_suffix_code,
        dpcm_residual, left, top, topleft, min_left_top, max_left_top, p, p2, output;
    h = 16 >> (n ? s->chroma_y_shift : 0);
    w = 16 >> (n ? s->chroma_x_shift : 0);

    block_mean = get_bits(&s->gb, s->avctx->bits_per_raw_sample);
    if (block_mean == 0){
        av_log(s->avctx, AV_LOG_ERROR, "Forbidden block_mean\n");
        return AVERROR_INVALIDDATA;
    }
    s->last_dc[n] = block_mean * (1 << (s->dct_precision + s->intra_dc_precision));

    rice_parameter = get_bits(&s->gb, 4);
    if (rice_parameter == 0) {
        av_log(s->avctx, AV_LOG_ERROR, "Forbidden rice_parameter\n");
        return AVERROR_INVALIDDATA;
    }

    if (rice_parameter == 15)
        rice_parameter = 0;

    if (rice_parameter > 11) {
        av_log(s->avctx, AV_LOG_ERROR, "Forbidden rice_parameter\n");
        return AVERROR_INVALIDDATA;
    }

    for (i = 0; i < h; i++) {
        output = 1 << (s->avctx->bits_per_raw_sample - 1);
        top = 1 << (s->avctx->bits_per_raw_sample - 1);

        for (j = 0; j < w; j++) {
            left = output;
            topleft = top;

            rice_prefix_code = get_unary(&s->gb, 1, 12);

            /* Escape */
            if (rice_prefix_code == 11)
                dpcm_residual = get_bits(&s->gb, s->avctx->bits_per_raw_sample);
            else {
                if (rice_prefix_code == 12) {
                    av_log(s->avctx, AV_LOG_ERROR, "Forbidden rice_prefix_code\n");
                    return AVERROR_INVALIDDATA;
                }
                rice_suffix_code = get_bitsz(&s->gb, rice_parameter);
                dpcm_residual = (rice_prefix_code << rice_parameter) + rice_suffix_code;
            }

            /* Map to a signed residual */
            if (dpcm_residual & 1)
                dpcm_residual = (-1 * dpcm_residual) >> 1;
            else
                dpcm_residual = (dpcm_residual >> 1);

            if (i != 0)
                top = macroblock[idx-w];

            p = left + top - topleft;
            min_left_top = FFMIN(left, top);
            if (p < min_left_top)
                p = min_left_top;

            max_left_top = FFMAX(left, top);
            if (p > max_left_top)
                p = max_left_top;

            p2 = (FFMIN(min_left_top, topleft) + FFMAX(max_left_top, topleft)) >> 1;
            if (p2 == p)
                p2 = block_mean;

            if (p2 > p)
                dpcm_residual *= -1;

            macroblock[idx++] = output = (dpcm_residual + p) & ((1 << s->avctx->bits_per_raw_sample) - 1);
        }
    }

    return 0;
}