static void naludmx_log_stats(GF_NALUDmxCtx *ctx)
{
	u32 i, count;
	const char *msg_import;
	u32 nb_frames = 0;
	if (ctx->cur_fps.den)
		nb_frames = (u32) (ctx->dts / ctx->cur_fps.den);

	if (ctx->dur.den && ctx->dur.num) {
		GF_LOG(GF_LOG_INFO, GF_LOG_MEDIA, ("%s duration specified at import time, may have parsed more frames than imported\n", ctx->log_name));
		msg_import = "parsed";
	} else {
		msg_import = "Import results:";
	}

	if (ctx->nb_si || ctx->nb_sp) {
		GF_LOG(GF_LOG_INFO, GF_LOG_MEDIA, ("%s %s %d frames (%d NALUs) - Slices: %d I %d P %d B %d SP %d SI - %d SEI - %d IDR\n", ctx->log_name, msg_import, nb_frames, ctx->nb_nalus, ctx->nb_i, ctx->nb_p, ctx->nb_b, ctx->nb_sp, ctx->nb_si, ctx->nb_sei, ctx->nb_idr ));
	} else if (ctx->vvc_no_stats) {
		GF_LOG(GF_LOG_INFO, GF_LOG_MEDIA, ("%s %s %d samples (%d NALUs) - %d SEI - %d IDR - %d CRA\n",
			                  ctx->log_name, msg_import, nb_frames, ctx->nb_nalus, ctx->nb_sei, ctx->nb_idr, ctx->nb_cra));
	} else {
		GF_LOG(GF_LOG_INFO, GF_LOG_MEDIA, ("%s %s %d samples (%d NALUs) - Slices: %d I %d P %d B - %d SEI - %d IDR - %d CRA\n",
			                  ctx->log_name, msg_import, nb_frames, ctx->nb_nalus, ctx->nb_i, ctx->nb_p, ctx->nb_b, ctx->nb_sei, ctx->nb_idr, ctx->nb_cra));
	}

	if (ctx->codecid==GF_CODECID_AVC) {
		count = gf_list_count(ctx->sps);
		for (i=0; i<count; i++) {
			AVC_SPS *sps;
			GF_NALUFFParam *svcc = (GF_NALUFFParam*)gf_list_get(ctx->sps, i);
			sps = & ctx->avc_state->sps[svcc->id];
			if (sps->nb_ei || sps->nb_ep) {
				GF_LOG(GF_LOG_INFO, GF_LOG_MEDIA, ("%s SVC (SSPS ID %d, %dx%d) %s Slices: %d I %d P %d B\n", ctx->log_name, svcc->id - GF_SVC_SSPS_ID_SHIFT, sps->width, sps->height, msg_import, sps->nb_ei, sps->nb_ep, sps->nb_eb ));
			}
		}
	} else if (ctx->nb_e_i || ctx->nb_e_p || ctx->nb_e_b) {
		GF_LOG(GF_LOG_INFO, GF_LOG_MEDIA, ("%s L-HEVC %s Slices: %d I %d P %d B\n", ctx->log_name, msg_import, ctx->nb_e_i, ctx->nb_e_p, ctx->nb_e_b ));
	}

	if (ctx->max_total_delay>1) {
		GF_LOG(GF_LOG_INFO, GF_LOG_MEDIA, ("%s Stream uses forward prediction - stream CTS offset: %d frames\n", ctx->log_name, ctx->max_total_delay));
	}

	if (!ctx->nal_adjusted) {
		if ((ctx->max_nalu_size < 0xFF) && (ctx->nal_length>1) ){
			GF_LOG(GF_LOG_INFO, GF_LOG_MEDIA, ("%s Max NALU size is %d - stream could be optimized by setting nal_length=1\n", ctx->log_name, ctx->max_nalu_size));
		} else if ((ctx->max_nalu_size < 0xFFFF) && (ctx->nal_length>2) ){
			GF_LOG(GF_LOG_INFO, GF_LOG_MEDIA, ("%s Max NALU size is %d - stream could be optimized by setting nal_length=2\n", ctx->log_name, ctx->max_nalu_size));
		}
	}
}