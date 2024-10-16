static GFINLINE void naludmx_update_time(GF_NALUDmxCtx *ctx)
{
	assert(ctx->cur_fps.num);

	if (!ctx->notime) {
		//very first frame, no dts diff, assume 3000/90k. It should only hurt if we have several frames packet in the first packet sent
		u64 dts_inc = ctx->cur_fps.den ? ctx->cur_fps.den : 3000;
		ctx->cts += dts_inc;
		ctx->dts += dts_inc;
	} else {
		assert(ctx->cur_fps.den);
		ctx->cts += ctx->cur_fps.den;
		ctx->dts += ctx->cur_fps.den;
	}
}