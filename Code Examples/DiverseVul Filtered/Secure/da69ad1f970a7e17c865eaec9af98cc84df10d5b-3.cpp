static GFINLINE void flac_dmx_update_cts(GF_FLACDmxCtx *ctx, u32 nb_samp)
{
	if (ctx->timescale) {
		u64 inc = nb_samp;
		inc *= ctx->timescale;
		inc /= ctx->sample_rate;
		ctx->cts += inc;
	} else {
		ctx->cts += nb_samp;
	}
}