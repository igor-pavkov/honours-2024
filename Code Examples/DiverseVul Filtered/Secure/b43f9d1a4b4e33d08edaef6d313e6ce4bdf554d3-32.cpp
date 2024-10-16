static void naludmx_update_clli_mdcv(GF_NALUDmxCtx *ctx, Bool reset_crc)
{
	if (!ctx->opid) return;

	if (reset_crc)
		ctx->clli_crc = 0;
	if ((ctx->hevc_state && ctx->hevc_state->clli_valid)
		|| (ctx->vvc_state && ctx->vvc_state->clli_valid)
	) {
		u8 *clli = ctx->hevc_state ? ctx->hevc_state->clli_data : ctx->vvc_state->clli_data;
		gf_filter_pid_set_property(ctx->opid, GF_PROP_PID_CONTENT_LIGHT_LEVEL, &PROP_DATA(clli, 4));
		ctx->clli_crc = gf_crc_32(clli, 4);
	}
	if (reset_crc)
		ctx->mdcv_crc = 0;

	if ((ctx->hevc_state && ctx->hevc_state->mdcv_valid)
		|| (ctx->vvc_state && ctx->vvc_state->mdcv_valid)
	) {
		u8 *mdcv = ctx->hevc_state ? ctx->hevc_state->mdcv_data : ctx->vvc_state->mdcv_data;
		gf_filter_pid_set_property(ctx->opid, GF_PROP_PID_MASTER_DISPLAY_COLOUR, &PROP_DATA(mdcv, 24));
		ctx->mdcv_crc = gf_crc_32(mdcv, 24);
	}
}