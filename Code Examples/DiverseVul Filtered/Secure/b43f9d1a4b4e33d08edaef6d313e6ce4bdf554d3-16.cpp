static void naludmx_update_nalu_maxsize(GF_NALUDmxCtx *ctx, u32 size)
{
	if (ctx->max_nalu_size < size) {
		ctx->max_nalu_size = size;
		if (size > ctx->max_nalu_size_allowed) {
			GF_LOG(GF_LOG_ERROR, GF_LOG_MEDIA, ("[%s] nal size %d larger than max allowed size %d - change import settings\n", ctx->log_name, size, ctx->max_nalu_size_allowed ));
		}
	}
}