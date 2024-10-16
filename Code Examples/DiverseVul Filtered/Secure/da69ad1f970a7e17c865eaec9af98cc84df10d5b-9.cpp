static GF_Err flac_dmx_initialize(GF_Filter *filter)
{
	GF_FLACDmxCtx *ctx = gf_filter_get_udta(filter);
	ctx->bs = gf_bs_new((u8 *)ctx, 1, GF_BITSTREAM_READ);
	return GF_OK;
}