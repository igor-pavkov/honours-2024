static void naldmx_switch_timestamps(GF_NALUDmxCtx *ctx, GF_FilterPacket *pck)
{
	//input pid sets some timescale - we flushed pending data , update cts
	if (!ctx->notime) {
		u64 ts = gf_filter_pck_get_cts(pck);
		if (ts != GF_FILTER_NO_TS) {
			ctx->prev_cts = ctx->cts;
			ctx->cts = ts;
		}
		ts = gf_filter_pck_get_dts(pck);
		if (ts != GF_FILTER_NO_TS) {
			if (ctx->full_au_source) {
				ctx->prev_dts = ctx->dts;
				ctx->dts = ts;
			} else {
				GF_FilterClockType ck_type = gf_filter_pid_get_clock_info(ctx->ipid, NULL, NULL);
				if (ck_type==GF_FILTER_CLOCK_PCR_DISC)
					ctx->dts = ts;
				else if (ctx->dts<ts)
					ctx->dts=ts;

				if (!ctx->prev_dts) ctx->prev_dts = ts;
				else if (ctx->prev_dts != ts) {
					u64 diff = ts;
					diff -= ctx->prev_dts;
					if (!ctx->cur_fps.den)
						ctx->cur_fps.den = (u32) diff;
					else if (ctx->cur_fps.den > diff)
						ctx->cur_fps.den = (u32) diff;

					ctx->prev_dts = ts;
				}
			}
		}
		ctx->pck_duration = gf_filter_pck_get_duration(pck);
		if (ctx->src_pck) gf_filter_pck_unref(ctx->src_pck);
		ctx->src_pck = pck;
		gf_filter_pck_ref_props(&ctx->src_pck);
		//store framing flags. If input_is_au_start, the first NAL of the first frame beginning in this packet will
		//use the DTS/CTS of the input packet, otherwise we will use our internal POC recompute
		gf_filter_pck_get_framing(pck, &ctx->input_is_au_start, NULL);
	}
}