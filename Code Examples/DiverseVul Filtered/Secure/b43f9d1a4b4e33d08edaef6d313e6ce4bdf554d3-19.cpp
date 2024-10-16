static void naludmx_finalize(GF_Filter *filter)
{
	GF_NALUDmxCtx *ctx = gf_filter_get_udta(filter);

	if (ctx->importer) naludmx_log_stats(ctx);

	if (ctx->bs_r) gf_bs_del(ctx->bs_r);
	if (ctx->bs_w) gf_bs_del(ctx->bs_w);
	if (ctx->indexes) gf_free(ctx->indexes);
	if (ctx->nal_store) gf_free(ctx->nal_store);
	if (ctx->pck_queue) {
		while (gf_list_count(ctx->pck_queue)) {
			GF_FilterPacket *pck = gf_list_pop_back(ctx->pck_queue);
			gf_filter_pck_discard(pck);
		}
		gf_list_del(ctx->pck_queue);
	}
	if (ctx->sei_buffer) gf_free(ctx->sei_buffer);
	if (ctx->svc_prefix_buffer) gf_free(ctx->svc_prefix_buffer);
	if (ctx->subsamp_buffer) gf_free(ctx->subsamp_buffer);

	if (ctx->src_pck) gf_filter_pck_unref(ctx->src_pck);
	ctx->src_pck = NULL;

	naludmx_reset_param_sets(ctx, GF_TRUE);

	if (ctx->avc_state) gf_free(ctx->avc_state);
	if (ctx->hevc_state) gf_free(ctx->hevc_state);
	if (ctx->vvc_state) gf_free(ctx->vvc_state);
}