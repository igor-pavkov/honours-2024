static void naludmx_set_hevc_linf(GF_NALUDmxCtx *ctx)
{
	u32 i, nb_layers=0, nb_sublayers=0;
	u8 *data;
	u32 data_size;
	GF_BitStream *bs;

	for (i=0; i<64; i++) {
		if (ctx->linf[i].layer_id_plus_one) nb_layers++;
		if (ctx->linf[i].min_temporal_id != ctx->linf[i].max_temporal_id) nb_sublayers++;
	}
	if (!nb_layers && !nb_sublayers)
		return;

	bs = gf_bs_new(NULL, 0, GF_BITSTREAM_WRITE);

	gf_bs_write_int(bs, 0, 2);
	gf_bs_write_int(bs, nb_layers, 6);
	for (i=0; i<nb_layers; i++) {
		if (! ctx->linf[i].layer_id_plus_one) continue;
		gf_bs_write_int(bs, 0, 4);
		gf_bs_write_int(bs, ctx->linf[i].layer_id_plus_one - 1, 6);
		gf_bs_write_int(bs, ctx->linf[i].min_temporal_id, 3);
		gf_bs_write_int(bs, ctx->linf[i].max_temporal_id, 3);
		gf_bs_write_int(bs, 0, 1);
		gf_bs_write_int(bs, 0xFF, 7);

	}
	gf_bs_get_content(bs, &data, &data_size);
	gf_bs_del(bs);
	gf_filter_pid_set_info_str(ctx->opid, "hevc:linf", &PROP_DATA_NO_COPY(data, data_size) );
}