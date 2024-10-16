static void latm_dmx_check_pid(GF_Filter *filter, GF_LATMDmxCtx *ctx)
{
	u8 *dsi_b;
	u32 dsi_s, sr, timescale=0;
	u32 codecid;
	if (!ctx->opid) {
		ctx->opid = gf_filter_pid_new(filter);
		gf_filter_pid_copy_properties(ctx->opid, ctx->ipid);
		latm_dmx_check_dur(filter, ctx);
	}
	if (!GF_M4ASampleRates[ctx->acfg.base_sr_index]) {
		GF_LOG(GF_LOG_ERROR, GF_LOG_PARSER, ("[LATMDmx] Wrong sample rate in audio config, broken stream\n"));
		ctx->in_error = GF_NON_COMPLIANT_BITSTREAM;
		return;
	}

	if ((ctx->sr_idx == ctx->acfg.base_sr_index) && (ctx->nb_ch == ctx->acfg.nb_chan )
		&& (ctx->base_object_type == ctx->acfg.base_object_type) ) return;

	if (ctx->acfg.base_object_type==GF_M4A_USAC)
		codecid = GF_CODECID_USAC;
	else
		codecid = GF_CODECID_AAC_MPEG4;
	//copy properties at init or reconfig
	gf_filter_pid_set_property(ctx->opid, GF_PROP_PID_STREAM_TYPE, & PROP_UINT( GF_STREAM_AUDIO));
	gf_filter_pid_set_property(ctx->opid, GF_PROP_PID_CODECID, & PROP_UINT( codecid));
	gf_filter_pid_set_property(ctx->opid, GF_PROP_PID_SAMPLES_PER_FRAME, & PROP_UINT(ctx->frame_size) );
	gf_filter_pid_set_property(ctx->opid, GF_PROP_PID_UNFRAMED, & PROP_BOOL(GF_FALSE) );
	if (ctx->is_file && ctx->index) {
		gf_filter_pid_set_property(ctx->opid, GF_PROP_PID_PLAYBACK_MODE, & PROP_UINT(GF_PLAYBACK_MODE_FASTFORWARD) );
	}
	if (ctx->duration.num)
		gf_filter_pid_set_property(ctx->opid, GF_PROP_PID_DURATION, & PROP_FRAC64(ctx->duration));


	ctx->nb_ch = ctx->acfg.nb_chan;
	ctx->base_object_type = ctx->acfg.base_object_type;

	sr = GF_M4ASampleRates[ctx->acfg.base_sr_index];
	if (!ctx->timescale) {
		//we change sample rate, change cts
		if (ctx->cts && (ctx->sr_idx != ctx->acfg.base_sr_index)) {
			ctx->cts *= sr;
			ctx->cts /= GF_M4ASampleRates[ctx->sr_idx];
		}
	}
	ctx->sr_idx = ctx->acfg.base_sr_index;

	ctx->dts_inc = ctx->frame_size;
	gf_m4a_write_config(&ctx->acfg, &dsi_b, &dsi_s);
	gf_filter_pid_set_property(ctx->opid, GF_PROP_PID_DECODER_CONFIG, & PROP_DATA_NO_COPY(dsi_b, dsi_s) );
	gf_filter_pid_set_property(ctx->opid, GF_PROP_PID_PROFILE_LEVEL, & PROP_UINT (ctx->acfg.audioPL) );
	gf_filter_pid_set_property(ctx->opid, GF_PROP_PID_SAMPLE_RATE, & PROP_UINT(sr));

	timescale = sr;

	gf_filter_pid_set_property(ctx->opid, GF_PROP_PID_TIMESCALE, & PROP_UINT(ctx->timescale ? ctx->timescale : timescale));
	gf_filter_pid_set_property(ctx->opid, GF_PROP_PID_NUM_CHANNELS, & PROP_UINT(ctx->nb_ch) );

	if (ctx->bitrate) {
		gf_filter_pid_set_property(ctx->opid, GF_PROP_PID_BITRATE, & PROP_UINT(ctx->bitrate));
	}
}