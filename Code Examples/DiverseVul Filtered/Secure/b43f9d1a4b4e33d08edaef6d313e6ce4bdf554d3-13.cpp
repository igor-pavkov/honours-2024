GF_Err naludmx_configure_pid(GF_Filter *filter, GF_FilterPid *pid, Bool is_remove)
{
	const GF_PropertyValue *p;
	u32 old_codecid;
	GF_NALUDmxCtx *ctx = gf_filter_get_udta(filter);

	if (is_remove) {
		ctx->ipid = NULL;
		if (ctx->opid) {
			gf_filter_pid_remove(ctx->opid);
			ctx->opid = NULL;
		}
		return GF_OK;
	}
	if (! gf_filter_pid_check_caps(pid))
		return GF_NOT_SUPPORTED;

	ctx->ipid = pid;
	p = gf_filter_pid_get_property(pid, GF_PROP_PID_TIMESCALE);
	if (p) {
		ctx->timescale = p->value.uint;
		//if we have a FPS prop, use it
		p = gf_filter_pid_get_property(pid, GF_PROP_PID_FPS);
		if (p) {
			ctx->cur_fps = p->value.frac;
		} else {
			ctx->cur_fps.den = 0;
			ctx->cur_fps.num = ctx->timescale;
		}
	}

	old_codecid = ctx->codecid;
	p = gf_filter_pid_get_property(pid, GF_PROP_PID_CODECID);
	if (p) {
		switch (p->value.uint) {
		case GF_CODECID_HEVC:
		case GF_CODECID_LHVC:
			ctx->codecid = GF_CODECID_HEVC;
			break;
		case GF_CODECID_VVC:
			ctx->codecid = GF_CODECID_VVC;
			break;
		case GF_CODECID_AVC:
		case GF_CODECID_AVC_PS:
		case GF_CODECID_SVC:
		case GF_CODECID_MVC:
			ctx->codecid = GF_CODECID_AVC;
			break;
		default:
			return GF_NOT_SUPPORTED;
		}
	}
	else {
		p = gf_filter_pid_get_property(pid, GF_PROP_PID_MIME);
		if (p && p->value.string && (
			strstr(p->value.string, "hvc")
			|| strstr(p->value.string, "hevc")
			|| strstr(p->value.string, "265")
			|| strstr(p->value.string, "shvc")
			|| strstr(p->value.string, "mhvc")
			|| strstr(p->value.string, "lhvc")
		) )
			ctx->codecid = GF_CODECID_HEVC;
		else if (p && p->value.string && (
			strstr(p->value.string, "vvc")
		) )
			ctx->codecid = GF_CODECID_VVC;
		else {
			p = gf_filter_pid_get_property(pid, GF_PROP_PID_FILE_EXT);
			if (p && p->value.string && (
				 strstr(p->value.string, "hvc")
				 || strstr(p->value.string, "hevc")
				 || strstr(p->value.string, "265")
				 || strstr(p->value.string, "shvc")
				 || strstr(p->value.string, "mhvc")
				 || strstr(p->value.string, "lhvc")
			 ) )
				ctx->codecid = GF_CODECID_HEVC;
			else if (p && p->value.string && (
				 strstr(p->value.string, "vvc")
				 || strstr(p->value.string, "266")
				 || strstr(p->value.string, "lvvc")
			 ) )
				ctx->codecid = GF_CODECID_VVC;
			else
				ctx->codecid = GF_CODECID_AVC;
		}
	}

	if (old_codecid && (old_codecid != ctx->codecid)) {
		naludmx_reset_param_sets(ctx, GF_FALSE);
	}

	if (ctx->codecid==GF_CODECID_HEVC) {
#ifdef GPAC_DISABLE_HEVC
		return GF_NOT_SUPPORTED;
#else
		ctx->log_name = "HEVC";
		if (ctx->avc_state) { gf_free(ctx->avc_state); ctx->avc_state = NULL; }
		if (ctx->vvc_state) { gf_free(ctx->vvc_state); ctx->vvc_state = NULL; }
		if (!ctx->hevc_state) GF_SAFEALLOC(ctx->hevc_state, HEVCState);
		ctx->min_layer_id = 0xFF;
#endif
	} else if (ctx->codecid==GF_CODECID_VVC) {
		ctx->log_name = "VVC";
		if (ctx->hevc_state) { gf_free(ctx->hevc_state); ctx->hevc_state = NULL; }
		if (ctx->avc_state) { gf_free(ctx->avc_state); ctx->avc_state = NULL; }
		if (!ctx->vvc_state) GF_SAFEALLOC(ctx->vvc_state, VVCState);
	} else {
		ctx->log_name = "AVC|H264";
		if (ctx->hevc_state) { gf_free(ctx->hevc_state); ctx->hevc_state = NULL; }
		if (ctx->vvc_state) { gf_free(ctx->vvc_state); ctx->vvc_state = NULL; }
		if (!ctx->avc_state) GF_SAFEALLOC(ctx->avc_state, AVCState);
	}
	if (ctx->timescale && !ctx->opid) {
		ctx->opid = gf_filter_pid_new(filter);
		ctx->first_slice_in_au = GF_TRUE;
	}
	ctx->full_au_source = GF_FALSE;
	p = gf_filter_pid_get_property(pid, GF_PROP_PID_UNFRAMED_FULL_AU);
	if (p && p->value.boolean) {
		GF_FilterEvent fevt;
		//this is a reframer used after an encoder, we want to make sure we have enough frames to compute POC otherwise we might block the chain
		//by holding input packets - ask 1s by default
		GF_FEVT_INIT(fevt, GF_FEVT_BUFFER_REQ, ctx->ipid);
		fevt.buffer_req.pid_only = GF_TRUE;
		fevt.buffer_req.max_buffer_us = 1000000;
		gf_filter_pid_send_event(ctx->ipid, &fevt);
		ctx->full_au_source = GF_TRUE;
	}

	//if source has no timescale, recompute time
	if (!ctx->timescale) ctx->notime = GF_TRUE;

	//copy properties at init or reconfig
	if (ctx->opid) {
		if (ctx->poc_probe_done) {
			//full frame mode, flush everything before signaling discontinuity
			//for other modes discontinuity we signal discontinuity before the current AU being reconstructed
			if (ctx->full_au_source && ctx->first_pck_in_au)
				naludmx_finalize_au_flags(ctx);

			naludmx_enqueue_or_dispatch(ctx, NULL, GF_TRUE);
		}
		ctx->nal_store_size = 0;
		gf_filter_pid_copy_properties(ctx->opid, ctx->ipid);
		//don't change codec type if reframing an ES (for HLS SAES)
		if (!ctx->timescale)
			gf_filter_pid_set_property(ctx->opid, GF_PROP_PID_STREAM_TYPE, & PROP_UINT(GF_STREAM_VISUAL));
		gf_filter_pid_set_property(ctx->opid, GF_PROP_PID_CODECID, & PROP_UINT(ctx->codecid));

		gf_filter_pid_set_property(ctx->opid, GF_PROP_PID_UNFRAMED, NULL);
		gf_filter_pid_set_property(ctx->opid, GF_PROP_PID_UNFRAMED_FULL_AU, NULL);
		if (!gf_filter_pid_get_property(ctx->ipid, GF_PROP_PID_ID))
			gf_filter_pid_set_property(ctx->opid, GF_PROP_PID_ID, &PROP_UINT(1));

		ctx->ps_modified = GF_TRUE;
		ctx->crc_cfg = ctx->crc_cfg_enh = 0;
	}

	return GF_OK;
}