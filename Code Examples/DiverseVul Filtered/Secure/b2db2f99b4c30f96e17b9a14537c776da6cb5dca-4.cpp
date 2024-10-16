static GFINLINE void latm_dmx_update_cts(GF_LATMDmxCtx *ctx)
{
	assert(ctx->dts_inc);

	if (ctx->timescale) {
		u64 inc = ctx->dts_inc;
		inc *= ctx->timescale;
		inc /= GF_M4ASampleRates[ctx->sr_idx];
		ctx->cts += inc;
	} else {
		ctx->cts += ctx->dts_inc;
	}
}