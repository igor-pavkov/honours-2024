static int mpeg4_decode_partition_a(Mpeg4DecContext *ctx)
{
    MpegEncContext *s = &ctx->m;
    int mb_num = 0;
    static const int8_t quant_tab[4] = { -1, -2, 1, 2 };

    /* decode first partition */
    s->first_slice_line = 1;
    for (; s->mb_y < s->mb_height; s->mb_y++) {
        ff_init_block_index(s);
        for (; s->mb_x < s->mb_width; s->mb_x++) {
            const int xy = s->mb_x + s->mb_y * s->mb_stride;
            int cbpc;
            int dir = 0;

            mb_num++;
            ff_update_block_index(s);
            if (s->mb_x == s->resync_mb_x && s->mb_y == s->resync_mb_y + 1)
                s->first_slice_line = 0;

            if (s->pict_type == AV_PICTURE_TYPE_I) {
                int i;

                do {
                    if (show_bits_long(&s->gb, 19) == DC_MARKER)
                        return mb_num - 1;

                    cbpc = get_vlc2(&s->gb, ff_h263_intra_MCBPC_vlc.table, INTRA_MCBPC_VLC_BITS, 2);
                    if (cbpc < 0) {
                        av_log(s->avctx, AV_LOG_ERROR,
                               "cbpc corrupted at %d %d\n", s->mb_x, s->mb_y);
                        return -1;
                    }
                } while (cbpc == 8);

                s->cbp_table[xy]               = cbpc & 3;
                s->current_picture.mb_type[xy] = MB_TYPE_INTRA;
                s->mb_intra                    = 1;

                if (cbpc & 4)
                    ff_set_qscale(s, s->qscale + quant_tab[get_bits(&s->gb, 2)]);

                s->current_picture.qscale_table[xy] = s->qscale;

                s->mbintra_table[xy] = 1;
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
                s->pred_dir_table[xy] = dir;
            } else { /* P/S_TYPE */
                int mx, my, pred_x, pred_y, bits;
                int16_t *const mot_val = s->current_picture.motion_val[0][s->block_index[0]];
                const int stride       = s->b8_stride * 2;

try_again:
                bits = show_bits(&s->gb, 17);
                if (bits == MOTION_MARKER)
                    return mb_num - 1;

                skip_bits1(&s->gb);
                if (bits & 0x10000) {
                    /* skip mb */
                    if (s->pict_type == AV_PICTURE_TYPE_S &&
                        ctx->vol_sprite_usage == GMC_SPRITE) {
                        s->current_picture.mb_type[xy] = MB_TYPE_SKIP  |
                                                         MB_TYPE_16x16 |
                                                         MB_TYPE_GMC   |
                                                         MB_TYPE_L0;
                        mx = get_amv(ctx, 0);
                        my = get_amv(ctx, 1);
                    } else {
                        s->current_picture.mb_type[xy] = MB_TYPE_SKIP  |
                                                         MB_TYPE_16x16 |
                                                         MB_TYPE_L0;
                        mx = my = 0;
                    }
                    mot_val[0]          =
                    mot_val[2]          =
                    mot_val[0 + stride] =
                    mot_val[2 + stride] = mx;
                    mot_val[1]          =
                    mot_val[3]          =
                    mot_val[1 + stride] =
                    mot_val[3 + stride] = my;

                    if (s->mbintra_table[xy])
                        ff_clean_intra_table_entries(s);
                    continue;
                }

                cbpc = get_vlc2(&s->gb, ff_h263_inter_MCBPC_vlc.table, INTER_MCBPC_VLC_BITS, 2);
                if (cbpc < 0) {
                    av_log(s->avctx, AV_LOG_ERROR,
                           "cbpc corrupted at %d %d\n", s->mb_x, s->mb_y);
                    return -1;
                }
                if (cbpc == 20)
                    goto try_again;

                s->cbp_table[xy] = cbpc & (8 + 3);  // 8 is dquant

                s->mb_intra = ((cbpc & 4) != 0);

                if (s->mb_intra) {
                    s->current_picture.mb_type[xy] = MB_TYPE_INTRA;
                    s->mbintra_table[xy] = 1;
                    mot_val[0]          =
                    mot_val[2]          =
                    mot_val[0 + stride] =
                    mot_val[2 + stride] = 0;
                    mot_val[1]          =
                    mot_val[3]          =
                    mot_val[1 + stride] =
                    mot_val[3 + stride] = 0;
                } else {
                    if (s->mbintra_table[xy])
                        ff_clean_intra_table_entries(s);

                    if (s->pict_type == AV_PICTURE_TYPE_S &&
                        ctx->vol_sprite_usage == GMC_SPRITE &&
                        (cbpc & 16) == 0)
                        s->mcsel = get_bits1(&s->gb);
                    else
                        s->mcsel = 0;

                    if ((cbpc & 16) == 0) {
                        /* 16x16 motion prediction */

                        ff_h263_pred_motion(s, 0, 0, &pred_x, &pred_y);
                        if (!s->mcsel) {
                            mx = ff_h263_decode_motion(s, pred_x, s->f_code);
                            if (mx >= 0xffff)
                                return -1;

                            my = ff_h263_decode_motion(s, pred_y, s->f_code);
                            if (my >= 0xffff)
                                return -1;
                            s->current_picture.mb_type[xy] = MB_TYPE_16x16 |
                                                             MB_TYPE_L0;
                        } else {
                            mx = get_amv(ctx, 0);
                            my = get_amv(ctx, 1);
                            s->current_picture.mb_type[xy] = MB_TYPE_16x16 |
                                                             MB_TYPE_GMC   |
                                                             MB_TYPE_L0;
                        }

                        mot_val[0]          =
                        mot_val[2]          =
                        mot_val[0 + stride] =
                        mot_val[2 + stride] = mx;
                        mot_val[1]          =
                        mot_val[3]          =
                        mot_val[1 + stride] =
                        mot_val[3 + stride] = my;
                    } else {
                        int i;
                        s->current_picture.mb_type[xy] = MB_TYPE_8x8 |
                                                         MB_TYPE_L0;
                        for (i = 0; i < 4; i++) {
                            int16_t *mot_val = ff_h263_pred_motion(s, i, 0, &pred_x, &pred_y);
                            mx = ff_h263_decode_motion(s, pred_x, s->f_code);
                            if (mx >= 0xffff)
                                return -1;

                            my = ff_h263_decode_motion(s, pred_y, s->f_code);
                            if (my >= 0xffff)
                                return -1;
                            mot_val[0] = mx;
                            mot_val[1] = my;
                        }
                    }
                }
            }
        }
        s->mb_x = 0;
    }

    return mb_num;
}