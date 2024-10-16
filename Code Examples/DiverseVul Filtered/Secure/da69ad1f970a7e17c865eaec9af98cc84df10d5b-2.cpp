static void flac_dmx_finalize(GF_Filter *filter)
{
	GF_FLACDmxCtx *ctx = gf_filter_get_udta(filter);
	if (ctx->bs) gf_bs_del(ctx->bs);
	if (ctx->indexes) gf_free(ctx->indexes);
	if (ctx->flac_buffer) gf_free(ctx->flac_buffer);
	if (ctx->src_pck) gf_filter_pck_unref(ctx->src_pck);
}