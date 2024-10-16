static int mpeg4_decode_partition_b(MpegEncContext *s, int mb_count)
{
    int mb_num = 0;
    static const int8_t quant_tab[4] = { -1, -2, 1, 2 };

    s->mb_x = s->resync_mb_x;
    s->first_slice_line = 1;
    for (s->mb_y = s->resync_mb_y; mb_num < mb_count; s->mb_y++) {
        ff_init_block_index(s);
        for (; mb_num < mb_count && s->mb_x < s->mb_width; s->mb_x++) {
            const int xy = s->mb_x + s->mb_y * s->mb_stride;

            mb_num++;
            ff_update_block_index(s);
            if (s->mb_x == s->resync_mb_x && s->mb_y == s->resync_mb_y + 1)
                s->first_slice_line = 0;

            if (s->pict_type == AV_PICTURE_TYPE_I) {
                int ac_pred = get_bits1(&s->gb);
                int cbpy    = get_vlc2(&s->gb, ff_h263_cbpy_vlc.table, CBPY_VLC_BITS, 1);
                if (cbpy < 0) {
                    av_log(s->avctx, AV_LOG_ERROR,
                           "cbpy corrupted at %d %d\n", s->mb_x, s->mb_y);
                    return -1;
                }

                s->cbp_table[xy]               |= cbpy << 2;
                s->current_picture.mb_type[xy] |= ac_pred * MB_TYPE_ACPRED;
            } else { /* P || S_TYPE */
                if (IS_INTRA(s->current_picture.mb_type[xy])) {
                    int i;
                    int dir     = 0;
                    int ac_pred = get_bits1(&s->gb);
                    int cbpy    = get_vlc2(&s->gb, ff_h263_cbpy_vlc.table, CBPY_VLC_BITS, 1);

                    if (cbpy < 0) {
                        av_log(s->avctx, AV_LOG_ERROR,
                               "I cbpy corrupted at %d %d\n", s->mb_x, s->mb_y);
                        return -1;
                    }

                    if (s->cbp_table[xy] & 8)
                        ff_set_qscale(s, s->qscale + quant_tab[get_bits(&s->gb, 2)]);
                    s->current_picture.qscale_table[xy] = s->qscale;

                    for (i = 0; i < 6; i++) {
                        int dc_pred_dir;
                        int dc = mpeg4_decode_dc(s, i, &dc_pred_dir);
                        if (dc < 0) {
                            av_log(s->avctx, AV_LOG_ERROR,
                                   "DC corrupted at %d %d\n", s->mb_x, s->mb_y);
                            return -1;
                        }
                        dir <<= 1;
                        if (dc_pred_dir)
                            dir |= 1;
                    }
                    s->cbp_table[xy]               &= 3;  // remove dquant
                    s->cbp_table[xy]               |= cbpy << 2;
                    s->current_picture.mb_type[xy] |= ac_pred * MB_TYPE_ACPRED;
                    s->pred_dir_table[xy]           = dir;
                } else if (IS_SKIP(s->current_picture.mb_type[xy])) {
                    s->current_picture.qscale_table[xy] = s->qscale;
                    s->cbp_table[xy]                    = 0;
                } else {
                    int cbpy = get_vlc2(&s->gb, ff_h263_cbpy_vlc.table, CBPY_VLC_BITS, 1);

                    if (cbpy < 0) {
                        av_log(s->avctx, AV_LOG_ERROR,
                               "P cbpy corrupted at %d %d\n", s->mb_x, s->mb_y);
                        return -1;
                    }

                    if (s->cbp_table[xy] & 8)
                        ff_set_qscale(s, s->qscale + quant_tab[get_bits(&s->gb, 2)]);
                    s->current_picture.qscale_table[xy] = s->qscale;

                    s->cbp_table[xy] &= 3;  // remove dquant
                    s->cbp_table[xy] |= (cbpy ^ 0xf) << 2;
                }
            }
        }
        if (mb_num >= mb_count)
            return 0;
        s->mb_x = 0;
    }
    return 0;
}