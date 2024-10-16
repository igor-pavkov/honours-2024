static av_cold int encode_init(AVCodecContext *avctx)
{
    MpegEncContext *s = avctx->priv_data;
    int ret;
    static int done = 0;

    if (avctx->width >= (1<<13) || avctx->height >= (1<<13)) {
        av_log(avctx, AV_LOG_ERROR, "dimensions too large for MPEG-4\n");
        return AVERROR(EINVAL);
    }

    if ((ret = ff_mpv_encode_init(avctx)) < 0)
        return ret;

    if (!done) {
        done = 1;

        init_uni_dc_tab();

        ff_rl_init(&ff_mpeg4_rl_intra, ff_mpeg4_static_rl_table_store[0]);

        init_uni_mpeg4_rl_tab(&ff_mpeg4_rl_intra, uni_mpeg4_intra_rl_bits, uni_mpeg4_intra_rl_len);
        init_uni_mpeg4_rl_tab(&ff_h263_rl_inter, uni_mpeg4_inter_rl_bits, uni_mpeg4_inter_rl_len);
    }

    s->min_qcoeff               = -2048;
    s->max_qcoeff               = 2047;
    s->intra_ac_vlc_length      = uni_mpeg4_intra_rl_len;
    s->intra_ac_vlc_last_length = uni_mpeg4_intra_rl_len + 128 * 64;
    s->inter_ac_vlc_length      = uni_mpeg4_inter_rl_len;
    s->inter_ac_vlc_last_length = uni_mpeg4_inter_rl_len + 128 * 64;
    s->luma_dc_vlc_length       = uni_DCtab_lum_len;
    s->ac_esc_length            = 7 + 2 + 1 + 6 + 1 + 12 + 1;
    s->y_dc_scale_table         = ff_mpeg4_y_dc_scale_table;
    s->c_dc_scale_table         = ff_mpeg4_c_dc_scale_table;

    if (s->avctx->flags & AV_CODEC_FLAG_GLOBAL_HEADER) {
        s->avctx->extradata = av_malloc(1024);
        init_put_bits(&s->pb, s->avctx->extradata, 1024);

        if (!(s->workaround_bugs & FF_BUG_MS))
            mpeg4_encode_visual_object_header(s);
        mpeg4_encode_vol_header(s, 0, 0);

//            ff_mpeg4_stuffing(&s->pb); ?
        flush_put_bits(&s->pb);
        s->avctx->extradata_size = (put_bits_count(&s->pb) + 7) >> 3;
    }
    return 0;
}