static inline int mpeg4_decode_dc(MpegEncContext *s, int n, int *dir_ptr)
{
    int level, code;

    if (n < 4)
        code = get_vlc2(&s->gb, dc_lum.table, DC_VLC_BITS, 1);
    else
        code = get_vlc2(&s->gb, dc_chrom.table, DC_VLC_BITS, 1);

    if (code < 0 || code > 9 /* && s->nbit < 9 */) {
        av_log(s->avctx, AV_LOG_ERROR, "illegal dc vlc\n");
        return -1;
    }

    if (code == 0) {
        level = 0;
    } else {
        if (IS_3IV1) {
            if (code == 1)
                level = 2 * get_bits1(&s->gb) - 1;
            else {
                if (get_bits1(&s->gb))
                    level = get_bits(&s->gb, code - 1) + (1 << (code - 1));
                else
                    level = -get_bits(&s->gb, code - 1) - (1 << (code - 1));
            }
        } else {
            level = get_xbits(&s->gb, code);
        }

        if (code > 8) {
            if (get_bits1(&s->gb) == 0) { /* marker */
                if (s->err_recognition & (AV_EF_BITSTREAM|AV_EF_COMPLIANT)) {
                    av_log(s->avctx, AV_LOG_ERROR, "dc marker bit missing\n");
                    return -1;
                }
            }
        }
    }

    return ff_mpeg4_pred_dc(s, n, level, dir_ptr, 0);
}