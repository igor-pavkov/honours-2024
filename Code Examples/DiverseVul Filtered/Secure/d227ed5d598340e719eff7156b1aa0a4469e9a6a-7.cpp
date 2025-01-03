static int mpeg4_decode_studio_block(MpegEncContext *s, int32_t block[64], int n)
{
    Mpeg4DecContext *ctx = s->avctx->priv_data;

    int cc, dct_dc_size, dct_diff, code, j, idx = 1, group = 0, run = 0,
        additional_code_len, sign, mismatch;
    VLC *cur_vlc = &ctx->studio_intra_tab[0];
    uint8_t *const scantable = s->intra_scantable.permutated;
    const uint16_t *quant_matrix;
    uint32_t flc;
    const int min = -1 *  (1 << (s->avctx->bits_per_raw_sample + 6));
    const int max =      ((1 << (s->avctx->bits_per_raw_sample + 6)) - 1);

    mismatch = 1;

    memset(block, 0, 64 * sizeof(int32_t));

    if (n < 4) {
        cc = 0;
        dct_dc_size = get_vlc2(&s->gb, ctx->studio_luma_dc.table, STUDIO_INTRA_BITS, 2);
        quant_matrix = s->intra_matrix;
    } else {
        cc = (n & 1) + 1;
        if (ctx->rgb)
            dct_dc_size = get_vlc2(&s->gb, ctx->studio_luma_dc.table, STUDIO_INTRA_BITS, 2);
        else
            dct_dc_size = get_vlc2(&s->gb, ctx->studio_chroma_dc.table, STUDIO_INTRA_BITS, 2);
        quant_matrix = s->chroma_intra_matrix;
    }

    if (dct_dc_size < 0) {
        av_log(s->avctx, AV_LOG_ERROR, "illegal dct_dc_size vlc\n");
        return AVERROR_INVALIDDATA;
    } else if (dct_dc_size == 0) {
        dct_diff = 0;
    } else {
        dct_diff = get_xbits(&s->gb, dct_dc_size);

        if (dct_dc_size > 8) {
            if(!check_marker(s->avctx, &s->gb, "dct_dc_size > 8"))
                return AVERROR_INVALIDDATA;
        }

    }

    s->last_dc[cc] += dct_diff;

    if (s->mpeg_quant)
        block[0] = s->last_dc[cc] * (8 >> s->intra_dc_precision);
    else
        block[0] = s->last_dc[cc] * (8 >> s->intra_dc_precision) * (8 >> s->dct_precision);
    /* TODO: support mpeg_quant for AC coefficients */

    block[0] = av_clip(block[0], min, max);
    mismatch ^= block[0];

    /* AC Coefficients */
    while (1) {
        group = get_vlc2(&s->gb, cur_vlc->table, STUDIO_INTRA_BITS, 2);

        if (group < 0) {
            av_log(s->avctx, AV_LOG_ERROR, "illegal ac coefficient group vlc\n");
            return AVERROR_INVALIDDATA;
        }

        additional_code_len = ac_state_tab[group][0];
        cur_vlc = &ctx->studio_intra_tab[ac_state_tab[group][1]];

        if (group == 0) {
            /* End of Block */
            break;
        } else if (group >= 1 && group <= 6) {
            /* Zero run length (Table B.47) */
            run = 1 << additional_code_len;
            if (additional_code_len)
                run += get_bits(&s->gb, additional_code_len);
            idx += run;
            continue;
        } else if (group >= 7 && group <= 12) {
            /* Zero run length and +/-1 level (Table B.48) */
            code = get_bits(&s->gb, additional_code_len);
            sign = code & 1;
            code >>= 1;
            run = (1 << (additional_code_len - 1)) + code;
            idx += run;
            if (idx > 63)
                return AVERROR_INVALIDDATA;
            j = scantable[idx++];
            block[j] = sign ? 1 : -1;
        } else if (group >= 13 && group <= 20) {
            /* Level value (Table B.49) */
            if (idx > 63)
                return AVERROR_INVALIDDATA;
            j = scantable[idx++];
            block[j] = get_xbits(&s->gb, additional_code_len);
        } else if (group == 21) {
            /* Escape */
            if (idx > 63)
                return AVERROR_INVALIDDATA;
            j = scantable[idx++];
            additional_code_len = s->avctx->bits_per_raw_sample + s->dct_precision + 4;
            flc = get_bits(&s->gb, additional_code_len);
            if (flc >> (additional_code_len-1))
                block[j] = -1 * (( flc ^ ((1 << additional_code_len) -1)) + 1);
            else
                block[j] = flc;
        }
        block[j] = ((8 * 2 * block[j] * quant_matrix[j] * s->qscale) >> s->dct_precision) / 32;
        block[j] = av_clip(block[j], min, max);
        mismatch ^= block[j];
    }

    block[63] ^= mismatch & 1;

    return 0;
}