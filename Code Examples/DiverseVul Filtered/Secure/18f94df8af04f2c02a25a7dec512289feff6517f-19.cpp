static av_cold void dprint_specific_config(ALSDecContext *ctx)
{
#ifdef DEBUG
    AVCodecContext *avctx    = ctx->avctx;
    ALSSpecificConfig *sconf = &ctx->sconf;

    av_dlog(avctx, "resolution = %i\n",           sconf->resolution);
    av_dlog(avctx, "floating = %i\n",             sconf->floating);
    av_dlog(avctx, "frame_length = %i\n",         sconf->frame_length);
    av_dlog(avctx, "ra_distance = %i\n",          sconf->ra_distance);
    av_dlog(avctx, "ra_flag = %i\n",              sconf->ra_flag);
    av_dlog(avctx, "adapt_order = %i\n",          sconf->adapt_order);
    av_dlog(avctx, "coef_table = %i\n",           sconf->coef_table);
    av_dlog(avctx, "long_term_prediction = %i\n", sconf->long_term_prediction);
    av_dlog(avctx, "max_order = %i\n",            sconf->max_order);
    av_dlog(avctx, "block_switching = %i\n",      sconf->block_switching);
    av_dlog(avctx, "bgmc = %i\n",                 sconf->bgmc);
    av_dlog(avctx, "sb_part = %i\n",              sconf->sb_part);
    av_dlog(avctx, "joint_stereo = %i\n",         sconf->joint_stereo);
    av_dlog(avctx, "mc_coding = %i\n",            sconf->mc_coding);
    av_dlog(avctx, "chan_config = %i\n",          sconf->chan_config);
    av_dlog(avctx, "chan_sort = %i\n",            sconf->chan_sort);
    av_dlog(avctx, "RLSLMS = %i\n",               sconf->rlslms);
    av_dlog(avctx, "chan_config_info = %i\n",     sconf->chan_config_info);
#endif
}