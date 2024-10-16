static void latm_dmx_finalize(GF_Filter *filter)
{
	GF_LATMDmxCtx *ctx = gf_filter_get_udta(filter);
	if (ctx->bs) gf_bs_del(ctx->bs);
	if (ctx->indexes) gf_free(ctx->indexes);
	if (ctx->latm_buffer) gf_free(ctx->latm_buffer);
}