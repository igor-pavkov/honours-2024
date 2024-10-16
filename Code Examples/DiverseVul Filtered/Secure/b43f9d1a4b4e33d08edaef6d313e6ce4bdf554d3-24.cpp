static void naludmx_reset_param_sets(GF_NALUDmxCtx *ctx, Bool do_free)
{
	naludmx_del_param_list(ctx->sps, do_free);
	naludmx_del_param_list(ctx->pps, do_free);
	naludmx_del_param_list(ctx->vps, do_free);
	naludmx_del_param_list(ctx->sps_ext, do_free);
	naludmx_del_param_list(ctx->pps_svc, do_free);
	naludmx_del_param_list(ctx->vvc_aps_pre, do_free);
	naludmx_del_param_list(ctx->vvc_dci, do_free);

}