static void naludmx_enqueue_or_dispatch(GF_NALUDmxCtx *ctx, GF_FilterPacket *n_pck, Bool flush_ref)
{
	//TODO: we are dispatching frames in "negctts mode", ie we may have DTS>CTS
	//need to signal this for consumers using DTS (eg MPEG-2 TS)
	if (flush_ref && ctx->pck_queue && ctx->poc_diff) {
		u32 dts_inc=0;
		s32 last_poc = 0;
		Bool patch_missing_frame = GF_FALSE;
		//send all reference packet queued
		if (ctx->strict_poc==STRICT_POC_ERROR) {
			u32 i;
			u32 nb_bframes = 0;
			for (i=0; i<gf_list_count(ctx->pck_queue); i++) {
				s32 poc;
				u64 poc_ts, dts;
				GF_FilterPacket *q_pck = gf_list_get(ctx->pck_queue, i);

				if (q_pck == ctx->first_pck_in_au) break;

				dts = gf_filter_pck_get_dts(q_pck);
				if (dts == GF_FILTER_NO_TS) continue;
				poc_ts = gf_filter_pck_get_cts(q_pck);
				assert(poc_ts != GF_FILTER_NO_TS);
				poc = (s32) ((s64) poc_ts - CTS_POC_OFFSET_SAFETY);

				if (i) {
					if (last_poc>poc) nb_bframes ++;
					else if (last_poc + ctx->poc_diff<poc)
						patch_missing_frame = GF_TRUE;
				}
				last_poc = poc;
			}
			if (nb_bframes>1)
				patch_missing_frame = GF_FALSE;
			else if (nb_bframes)
				patch_missing_frame = GF_TRUE;
		}
		last_poc = GF_INT_MIN;

		while (gf_list_count(ctx->pck_queue) ) {
			u64 dts;
			GF_FilterPacket *q_pck = gf_list_get(ctx->pck_queue, 0);

			if (q_pck == ctx->first_pck_in_au) break;

			dts = gf_filter_pck_get_dts(q_pck);
			if (dts != GF_FILTER_NO_TS) {
				s32 poc;
				u64 poc_ts, cts;
				u8 carousel_info = gf_filter_pck_get_carousel_version(q_pck);

				//we reused timing from source packets
				if (!carousel_info) {
					assert(ctx->timescale);
					gf_list_rem(ctx->pck_queue, 0);
					gf_filter_pck_send(q_pck);
					continue;
				}
				gf_filter_pck_set_carousel_version(q_pck, 0);


				poc_ts = gf_filter_pck_get_cts(q_pck);
				assert(poc_ts != GF_FILTER_NO_TS);
				poc = (s32) ((s64) poc_ts - CTS_POC_OFFSET_SAFETY);

				if (patch_missing_frame) {
					if (last_poc!=GF_INT_MIN) {
						//check if we missed an IDR (poc reset)
						if (poc && (last_poc > poc) ) {
							last_poc = 0;
							dts_inc += ctx->cur_fps.den;
							ctx->dts_last_IDR = dts;
							ctx->dts += ctx->cur_fps.den;
						}
						//check if we miss a frame
						while (last_poc + ctx->poc_diff < poc) {
							last_poc += ctx->poc_diff;
							dts_inc += ctx->cur_fps.den;
							ctx->dts += ctx->cur_fps.den;
						}
					}
					last_poc = poc;
					dts += dts_inc;
				}
				//poc is stored as diff since last IDR which has min_poc
				cts = ( (ctx->min_poc + (s32) poc) * ctx->cur_fps.den ) / ctx->poc_diff + ctx->dts_last_IDR;

				/*if PAFF, 2 pictures (eg poc) <=> 1 aggregated frame (eg sample), divide by 2*/
				if (ctx->is_paff) {
					cts /= 2;
					/*in some cases the poc is not on the top field - if that is the case, round up*/
					if (cts % ctx->cur_fps.den) {
						cts = ((cts/ctx->cur_fps.den)+1) * ctx->cur_fps.den;
					}
				}

				gf_filter_pck_set_cts(q_pck, cts);
				GF_LOG(GF_LOG_DEBUG, GF_LOG_MEDIA, ("[%s] Frame timestamps computed dts "LLU" cts "LLU" (poc %d min poc %d poc_diff %d last IDR DTS "LLU")\n", ctx->log_name, dts, cts, poc, ctx->min_poc, ctx->poc_diff, ctx->dts_last_IDR));

				if (ctx->importer && ctx->cur_fps.den) {
					poc = (s32) ( (s64) cts - (s64) dts);
					if (poc<0) poc = -poc;
					poc /= ctx->cur_fps.den;
					if (poc > ctx->max_total_delay)
						ctx->max_total_delay = poc;
				}
			}
			gf_list_rem(ctx->pck_queue, 0);
			gf_filter_pck_send(q_pck);
		}
	}
	if (!n_pck) return;

	if (!ctx->pck_queue) ctx->pck_queue = gf_list_new();
	gf_list_add(ctx->pck_queue, n_pck);
}