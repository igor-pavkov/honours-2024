static GF_Err naludmx_initialize(GF_Filter *filter)
{
	GF_NALUDmxCtx *ctx = gf_filter_get_udta(filter);
	ctx->sps = gf_list_new();
	ctx->pps = gf_list_new();
	switch (ctx->nal_length) {
	case 1:
		ctx->max_nalu_size_allowed = 0xFF;
		break;
	case 2:
		ctx->max_nalu_size_allowed = 0xFFFF;
		break;
	case 4:
		ctx->max_nalu_size_allowed = 0xFFFFFFFF;
		break;
	case 0:
		ctx->max_nalu_size_allowed = 0xFFFFFFFF;
		ctx->nal_length = 4;
		ctx->nal_adjusted = GF_TRUE;
		break;
	default:
		GF_LOG(GF_LOG_WARNING, GF_LOG_MEDIA, ("[%s] NAL size length %d is not allowed, defaulting to 4 bytes\n", ctx->log_name));
		ctx->max_nalu_size_allowed = 0xFFFFFFFF;
		ctx->nal_length = 4;
		break;
	}
	return GF_OK;
}