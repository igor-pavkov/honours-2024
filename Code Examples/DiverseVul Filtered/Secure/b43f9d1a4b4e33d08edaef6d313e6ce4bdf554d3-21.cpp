GF_FilterPacket *naludmx_start_nalu(GF_NALUDmxCtx *ctx, u32 nal_size, Bool skip_nal_field, Bool *au_start, u8 **pck_data)
{
	GF_FilterPacket *dst_pck = gf_filter_pck_new_alloc(ctx->opid, nal_size + (skip_nal_field ? 0 : ctx->nal_length), pck_data);
	if (!dst_pck) return NULL;

	if (!skip_nal_field) {
		if (!ctx->bs_w) ctx->bs_w = gf_bs_new(*pck_data, ctx->nal_length, GF_BITSTREAM_WRITE);
		else gf_bs_reassign_buffer(ctx->bs_w, *pck_data, ctx->nal_length);
		gf_bs_write_int(ctx->bs_w, nal_size, 8*ctx->nal_length);
	}

	if (*au_start) {
		ctx->first_pck_in_au = dst_pck;
		if (ctx->src_pck) gf_filter_pck_merge_properties(ctx->src_pck, dst_pck);

		gf_filter_pck_set_framing(dst_pck, GF_TRUE, GF_FALSE);
		//we reuse the timing of the input packet for the first nal of the first frame starting in this packet
		if (ctx->input_is_au_start) {
			ctx->input_is_au_start = GF_FALSE;
			gf_filter_pck_set_dts(dst_pck, ctx->dts);
			gf_filter_pck_set_cts(dst_pck, ctx->cts);
		} else {
			//we don't set the CTS, it will be set once we detect frame end
			gf_filter_pck_set_dts(dst_pck, ctx->dts);
		}
		//we use the carousel flag temporarly to indicate the cts must be recomputed
		gf_filter_pck_set_carousel_version(dst_pck, ctx->notime ? 1 : 0);

		gf_filter_pck_set_duration(dst_pck, ctx->pck_duration ? ctx->pck_duration : ctx->cur_fps.den);
		if (ctx->in_seek) {
			gf_filter_pck_set_seek_flag(dst_pck, GF_TRUE);
			if (ctx->first_gdr) {
				ctx->first_gdr = GF_FALSE;
				gf_filter_pck_set_sap(ctx->first_pck_in_au, GF_FILTER_SAP_4);
			}
		}

		naludmx_update_time(ctx);
		*au_start = GF_FALSE;
		ctx->nb_frames++;
	} else {
		gf_filter_pck_set_framing(dst_pck, GF_FALSE, GF_FALSE);
	}
	naludmx_update_nalu_maxsize(ctx, nal_size);

	naludmx_enqueue_or_dispatch(ctx, dst_pck, GF_FALSE);

	return dst_pck;
}