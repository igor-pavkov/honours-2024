static void naludmx_end_access_unit(GF_NALUDmxCtx *ctx)
{
	//finalize current fram flags - we will flush(send) later on
	naludmx_finalize_au_flags(ctx);

	ctx->has_islice = GF_FALSE;
	ctx->first_slice_in_au = GF_TRUE;
	ctx->sei_recovery_frame_count = -1;
	ctx->au_sap = GF_FILTER_SAP_NONE;
	ctx->au_sap2_poc_reset = GF_FALSE;
	ctx->bottom_field_flag = GF_FALSE;
}