int ff_mpeg4_decode_partitions(Mpeg4DecContext *ctx)
{
    MpegEncContext *s = &ctx->m;
    int mb_num;
    const int part_a_error = s->pict_type == AV_PICTURE_TYPE_I ? (ER_DC_ERROR | ER_MV_ERROR) : ER_MV_ERROR;
    const int part_a_end   = s->pict_type == AV_PICTURE_TYPE_I ? (ER_DC_END   | ER_MV_END)   : ER_MV_END;

    mb_num = mpeg4_decode_partition_a(ctx);
    if (mb_num < 0) {
        ff_er_add_slice(&s->er, s->resync_mb_x, s->resync_mb_y,
                        s->mb_x, s->mb_y, part_a_error);
        return -1;
    }

    if (s->resync_mb_x + s->resync_mb_y * s->mb_width + mb_num > s->mb_num) {
        av_log(s->avctx, AV_LOG_ERROR, "slice below monitor ...\n");
        ff_er_add_slice(&s->er, s->resync_mb_x, s->resync_mb_y,
                        s->mb_x, s->mb_y, part_a_error);
        return -1;
    }

    s->mb_num_left = mb_num;

    if (s->pict_type == AV_PICTURE_TYPE_I) {
        while (show_bits(&s->gb, 9) == 1)
            skip_bits(&s->gb, 9);
        if (get_bits_long(&s->gb, 19) != DC_MARKER) {
            av_log(s->avctx, AV_LOG_ERROR,
                   "marker missing after first I partition at %d %d\n",
                   s->mb_x, s->mb_y);
            return -1;
        }
    } else {
        while (show_bits(&s->gb, 10) == 1)
            skip_bits(&s->gb, 10);
        if (get_bits(&s->gb, 17) != MOTION_MARKER) {
            av_log(s->avctx, AV_LOG_ERROR,
                   "marker missing after first P partition at %d %d\n",
                   s->mb_x, s->mb_y);
            return -1;
        }
    }
    ff_er_add_slice(&s->er, s->resync_mb_x, s->resync_mb_y,
                    s->mb_x - 1, s->mb_y, part_a_end);

    if (mpeg4_decode_partition_b(s, mb_num) < 0) {
        if (s->pict_type == AV_PICTURE_TYPE_P)
            ff_er_add_slice(&s->er, s->resync_mb_x, s->resync_mb_y,
                            s->mb_x, s->mb_y, ER_DC_ERROR);
        return -1;
    } else {
        if (s->pict_type == AV_PICTURE_TYPE_P)
            ff_er_add_slice(&s->er, s->resync_mb_x, s->resync_mb_y,
                            s->mb_x - 1, s->mb_y, ER_DC_END);
    }

    return 0;
}