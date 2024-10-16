static Bool naludmx_process_event(GF_Filter *filter, const GF_FilterEvent *evt)
{
	u32 i;
	u64 file_pos = 0;
	GF_FilterEvent fevt;
	GF_NALUDmxCtx *ctx = gf_filter_get_udta(filter);

	switch (evt->base.type) {
	case GF_FEVT_PLAY:
		if (!ctx->is_playing) {
			ctx->is_playing = GF_TRUE;
			ctx->cts = ctx->dts = 0;
		}
		if (! ctx->is_file) {
			if (!ctx->initial_play_done) {
				ctx->initial_play_done = GF_TRUE;
				if (evt->play.start_range<0.1)
					return GF_FALSE;
			}
			ctx->resume_from = 0;
			ctx->nal_store_size = 0;
			return GF_FALSE;
		}
		if (ctx->start_range && (ctx->index<0)) {
			ctx->index = -ctx->index;
			ctx->file_loaded = GF_FALSE;
			ctx->duration.den = ctx->duration.num = 0;
			GF_LOG(GF_LOG_INFO, GF_LOG_MEDIA, ("[%s] Play request from %d, building index\n", ctx->log_name, ctx->start_range));
			naludmx_check_dur(filter, ctx);
		}
		ctx->start_range = evt->play.start_range;
		ctx->in_seek = GF_TRUE;

		if (ctx->start_range) {
			ctx->nb_nalus = ctx->nb_i = ctx->nb_p = ctx->nb_b = ctx->nb_sp = ctx->nb_si = ctx->nb_sei = ctx->nb_idr = ctx->nb_cra = 0;
			for (i=1; i<ctx->index_size; i++) {
				if (ctx->indexes[i].duration>ctx->start_range) {
					ctx->cts = (u64) (ctx->indexes[i-1].duration * ctx->cur_fps.num);
					ctx->dts = ctx->dts_last_IDR = ctx->cts;
					file_pos = ctx->indexes[i-1].pos;
					ctx->seek_gdr_count = ctx->indexes[i-1].roll_count;
					if (ctx->seek_gdr_count) ctx->first_gdr = GF_TRUE;
					break;
				}
			}
		}
		if (!ctx->initial_play_done) {
			ctx->initial_play_done = GF_TRUE;
			//seek will not change the current source state, don't send a seek
			if (!file_pos) {
				//very short streams, input is done before we get notified for play and everything stored in memory: flush
				if (gf_filter_pid_is_eos(ctx->ipid) && (ctx->nal_store_size)) {
					gf_filter_post_process_task(filter);
				}
				return GF_TRUE;
			}
		}
		ctx->nb_frames = 0;
		ctx->nb_nalus = 0;
		ctx->resume_from = 0;
		ctx->nal_store_size = 0;

		//post a seek
		GF_FEVT_INIT(fevt, GF_FEVT_SOURCE_SEEK, ctx->ipid);
		fevt.seek.start_offset = file_pos;
		gf_filter_pid_send_event(ctx->ipid, &fevt);

		//cancel event
		return GF_TRUE;

	case GF_FEVT_STOP:
		//don't cancel event
		ctx->is_playing = GF_FALSE;
		ctx->nal_store_size = 0;
		ctx->resume_from = 0;
		ctx->cts = 0;
		return GF_FALSE;

	case GF_FEVT_SET_SPEED:
		//cancel event
		return GF_TRUE;
	default:
		break;
	}
	//by default don't cancel event - to rework once we have downloading in place
	return GF_FALSE;
}