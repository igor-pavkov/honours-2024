static void latm_dmx_check_dur(GF_Filter *filter, GF_LATMDmxCtx *ctx)
{
	FILE *stream;
	GF_BitStream *bs;
	GF_M4ADecSpecInfo acfg;
	u64 duration, cur_dur, cur_pos, rate;
	s32 sr_idx = -1;
	const GF_PropertyValue *p;
	if (!ctx->opid || ctx->timescale || ctx->file_loaded) return;

	if (ctx->index<=0) {
		ctx->file_loaded = GF_TRUE;
		return;
	}

	p = gf_filter_pid_get_property(ctx->ipid, GF_PROP_PID_FILEPATH);
	if (!p || !p->value.string || !strncmp(p->value.string, "gmem://", 7)) {
		ctx->is_file = GF_FALSE;
		ctx->file_loaded = GF_TRUE;
		return;
	}
	ctx->is_file = GF_TRUE;

	stream = gf_fopen(p->value.string, "rb");
	if (!stream) return;

	ctx->index_size = 0;

	memset(&acfg, 0, sizeof(GF_M4ADecSpecInfo));


	bs = gf_bs_from_file(stream, GF_BITSTREAM_READ);
	duration = 0;
	cur_dur = 0;
	cur_pos = gf_bs_get_position(bs);
	while (latm_dmx_sync_frame_bs(bs, &acfg, 0, NULL, NULL)) {
		if ((sr_idx>=0) && (sr_idx != acfg.base_sr_index)) {
			duration *= GF_M4ASampleRates[acfg.base_sr_index];
			duration /= GF_M4ASampleRates[sr_idx];

			cur_dur *= GF_M4ASampleRates[acfg.base_sr_index];
			cur_dur /= GF_M4ASampleRates[sr_idx];
		}
		sr_idx = acfg.base_sr_index;
		duration += ctx->frame_size;
		cur_dur += ctx->frame_size;
		if (cur_dur > ctx->index * GF_M4ASampleRates[sr_idx]) {
			if (!ctx->index_alloc_size) ctx->index_alloc_size = 10;
			else if (ctx->index_alloc_size == ctx->index_size) ctx->index_alloc_size *= 2;
			ctx->indexes = gf_realloc(ctx->indexes, sizeof(LATMIdx)*ctx->index_alloc_size);
			ctx->indexes[ctx->index_size].pos = cur_pos;
			ctx->indexes[ctx->index_size].duration = (Double) duration;
			ctx->indexes[ctx->index_size].duration /= GF_M4ASampleRates[sr_idx];
			ctx->index_size ++;
			cur_dur = 0;
		}

		cur_pos = gf_bs_get_position(bs);
	}
	rate = gf_bs_get_position(bs);
	gf_bs_del(bs);
	gf_fclose(stream);

	if (sr_idx>=0) {
		if (!ctx->duration.num || (ctx->duration.num  * GF_M4ASampleRates[sr_idx] != duration * ctx->duration.den)) {
			ctx->duration.num = (s32) duration;
			ctx->duration.den = GF_M4ASampleRates[sr_idx];

			gf_filter_pid_set_property(ctx->opid, GF_PROP_PID_DURATION, & PROP_FRAC64(ctx->duration));

			if (duration && !gf_sys_is_test_mode() ) {
				rate *= 8 * ctx->duration.den;
				rate /= ctx->duration.num;
				ctx->bitrate = (u32) rate;
			}
		}
	}
	p = gf_filter_pid_get_property(ctx->ipid, GF_PROP_PID_FILE_CACHED);
	if (p && p->value.boolean) ctx->file_loaded = GF_TRUE;
	gf_filter_pid_set_property(ctx->opid, GF_PROP_PID_CAN_DATAREF, & PROP_BOOL(GF_TRUE ) );
}