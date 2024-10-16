static int mpeg4_decode_partitioned_mb(MpegEncContext *s, int16_t block[6][64])
{
    Mpeg4DecContext *ctx = (Mpeg4DecContext *)s;
    int cbp, mb_type;
    const int xy = s->mb_x + s->mb_y * s->mb_stride;

    mb_type = s->current_picture.mb_type[xy];
    cbp     = s->cbp_table[xy];

    ctx->use_intra_dc_vlc = s->qscale < ctx->intra_dc_threshold;

    if (s->current_picture.qscale_table[xy] != s->qscale)
        ff_set_qscale(s, s->current_picture.qscale_table[xy]);

    if (s->pict_type == AV_PICTURE_TYPE_P ||
        s->pict_type == AV_PICTURE_TYPE_S) {
        int i;
        for (i = 0; i < 4; i++) {
            s->mv[0][i][0] = s->current_picture.motion_val[0][s->block_index[i]][0];
            s->mv[0][i][1] = s->current_picture.motion_val[0][s->block_index[i]][1];
        }
        s->mb_intra = IS_INTRA(mb_type);

        if (IS_SKIP(mb_type)) {
            /* skip mb */
            for (i = 0; i < 6; i++)
                s->block_last_index[i] = -1;
            s->mv_dir  = MV_DIR_FORWARD;
            s->mv_type = MV_TYPE_16X16;
            if (s->pict_type == AV_PICTURE_TYPE_S
                && ctx->vol_sprite_usage == GMC_SPRITE) {
                s->mcsel      = 1;
                s->mb_skipped = 0;
            } else {
                s->mcsel      = 0;
                s->mb_skipped = 1;
            }
        } else if (s->mb_intra) {
            s->ac_pred = IS_ACPRED(s->current_picture.mb_type[xy]);
        } else if (!s->mb_intra) {
            // s->mcsel = 0;  // FIXME do we need to init that?

            s->mv_dir = MV_DIR_FORWARD;
            if (IS_8X8(mb_type)) {
                s->mv_type = MV_TYPE_8X8;
            } else {
                s->mv_type = MV_TYPE_16X16;
            }
        }
    } else { /* I-Frame */
        s->mb_intra = 1;
        s->ac_pred  = IS_ACPRED(s->current_picture.mb_type[xy]);
    }

    if (!IS_SKIP(mb_type)) {
        int i;
        s->dsp.clear_blocks(s->block[0]);
        /* decode each block */
        for (i = 0; i < 6; i++) {
            if (mpeg4_decode_block(ctx, block[i], i, cbp & 32, s->mb_intra, ctx->rvlc) < 0) {
                av_log(s->avctx, AV_LOG_ERROR,
                       "texture corrupted at %d %d %d\n",
                       s->mb_x, s->mb_y, s->mb_intra);
                return -1;
            }
            cbp += cbp;
        }
    }

    /* per-MB end of slice check */
    if (--s->mb_num_left <= 0) {
        if (mpeg4_is_resync(ctx))
            return SLICE_END;
        else
            return SLICE_NOEND;
    } else {
        if (mpeg4_is_resync(ctx)) {
            const int delta = s->mb_x + 1 == s->mb_width ? 2 : 1;
            if (s->cbp_table[xy + delta])
                return SLICE_END;
        }
        return SLICE_OK;
    }
}