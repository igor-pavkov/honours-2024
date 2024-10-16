static void av1_parse_uncompressed_header(GF_BitStream *bs, AV1State *state)
{
	Bool error_resilient_mode = GF_FALSE, allow_screen_content_tools = GF_FALSE, force_integer_mv = GF_FALSE;
	Bool /*use_ref_frame_mvs = GF_FALSE,*/ FrameIsIntra = GF_FALSE, frame_size_override_flag = GF_FALSE;
	Bool disable_cdf_update = GF_FALSE;
	u8 showable_frame;
	u8 primary_ref_frame;
	u16 idLen = 0;
	u32 idx;
	s8 ref_frame_idx[AV1_REFS_PER_FRAME];
	AV1StateFrame *frame_state = &state->frame_state;

	if (state->frame_id_numbers_present_flag) {
		idLen = (state->additional_frame_id_length_minus_1 + state->delta_frame_id_length_minus_2 + 3);
	}
	frame_state->refresh_frame_flags = 0;

	showable_frame = 0;
	if (state->reduced_still_picture_header) {
		frame_state->key_frame = GF_TRUE;
		FrameIsIntra = GF_TRUE;
		frame_state->frame_type = AV1_KEY_FRAME;
		frame_state->show_frame = GF_TRUE;
		frame_state->show_existing_frame = 0;
	}
	else {
		frame_state->show_existing_frame = gf_bs_read_int_log(bs, 1, "show_existing_frame");
		if (frame_state->show_existing_frame == GF_TRUE) {
			frame_state->frame_to_show_map_idx = gf_bs_read_int_log(bs, 3, "frame_to_show_map_idx");
			frame_state->frame_type = state->RefFrameType[frame_state->frame_to_show_map_idx];

			if (state->decoder_model_info_present_flag && !state->equal_picture_interval) {
				gf_bs_read_int_log(bs, state->frame_presentation_time_length, "frame_presentation_time");
			}

			frame_state->refresh_frame_flags = 0;
			if (state->frame_id_numbers_present_flag) {
				gf_bs_read_int_log(bs, idLen, "display_frame_id");
			}
			if (frame_state->frame_type == AV1_KEY_FRAME) {
				frame_state->refresh_frame_flags = AV1_ALL_FRAMES;
			}
			/*
			if (film_grain_params_present) {
				load_grain_params(frame_to_show_map_idx)
			}*/
			return;
		}
		frame_state->frame_type = gf_bs_read_int_log(bs, 2, "frame_type");
		FrameIsIntra = (frame_state->frame_type == AV1_INTRA_ONLY_FRAME || frame_state->frame_type == AV1_KEY_FRAME);
		frame_state->show_frame = gf_bs_read_int_log(bs, 1, "show_frame");
		if (frame_state->is_first_frame) {
			frame_state->key_frame = frame_state->seen_seq_header && frame_state->show_frame && frame_state->frame_type == AV1_KEY_FRAME && frame_state->seen_frame_header;
		}
		if (frame_state->show_frame && state->decoder_model_info_present_flag && !state->equal_picture_interval) {
			gf_bs_read_int_log(bs, state->frame_presentation_time_length, "frame_presentation_time");
		}
		if (frame_state->show_frame) {
			showable_frame = frame_state->frame_type != AV1_KEY_FRAME;

		}
		else {
			showable_frame = gf_bs_read_int_log(bs, 1, "showable_frame");
		}
		if (frame_state->frame_type == AV1_SWITCH_FRAME || (frame_state->frame_type == AV1_KEY_FRAME && frame_state->show_frame))
			error_resilient_mode = GF_TRUE;
		else
			error_resilient_mode = gf_bs_read_int_log(bs, 1, "error_resilient_mode");
	}

	if ((frame_state->frame_type == AV1_KEY_FRAME) && frame_state->show_frame) {
		u32 i;
		for (i = 0; i < AV1_NUM_REF_FRAMES; i++) {
			state->RefValid[i] = 0;
			state->RefOrderHint[i] = 0;
		}
		for (i = 0; i < AV1_REFS_PER_FRAME; i++) {
			state->OrderHints[AV1_LAST_FRAME + i] = 0;
		}
	}

	disable_cdf_update = gf_bs_read_int_log(bs, 1, "disable_cdf_update");
	if (state->seq_force_screen_content_tools == 2/*SELECT_SCREEN_CONTENT_TOOLS*/) {
		allow_screen_content_tools = gf_bs_read_int_log(bs, 1, "allow_screen_content_tools");
	}
	else {
		allow_screen_content_tools = state->seq_force_screen_content_tools;
	}
	if (allow_screen_content_tools) {
		if (state->seq_force_integer_mv == 2/*SELECT_INTEGER_MV*/) {
			force_integer_mv = gf_bs_read_int_log(bs, 1, "force_integer_mv");
		}
		else {
			force_integer_mv = state->seq_force_integer_mv;
		}
	}
	else {
		force_integer_mv = 0;
	}
	if (FrameIsIntra) {
		force_integer_mv = 1;
	}
	if (state->frame_id_numbers_present_flag) {
		gf_bs_read_int_log(bs, idLen, "current_frame_id");
	}
	if (frame_state->frame_type == AV1_SWITCH_FRAME)
		frame_size_override_flag = GF_TRUE;
	else if (state->reduced_still_picture_header)
		frame_size_override_flag = GF_FALSE;
	else
		frame_size_override_flag = gf_bs_read_int_log(bs, 1, "frame_size_override_flag");

	frame_state->order_hint = gf_bs_read_int_log(bs, state->OrderHintBits, "order_hint");
	if (FrameIsIntra || error_resilient_mode) {
		primary_ref_frame = AV1_PRIMARY_REF_NONE;
	}
	else {
		primary_ref_frame = gf_bs_read_int_log(bs, 3, "primary_ref_frame");
	}

	if (state->decoder_model_info_present_flag) {
		u8 buffer_removal_time_present_flag = gf_bs_read_int_log(bs, 1, "buffer_removal_time_present_flag");
		if (buffer_removal_time_present_flag) {
			u32 opNum;
			for (opNum = 0; opNum < state->operating_points_count; opNum++) {
				if (state->decoder_model_present_for_this_op[opNum]) {
					u8 opPtIdc = state->operating_point_idc[opNum];
					u8 inTemporalLayer = (opPtIdc >> state->temporal_id) & 1;
					u8 inSpatialLayer = (opPtIdc >> (state->spatial_id + 8)) & 1;
					if (opPtIdc == 0 || (inTemporalLayer && inSpatialLayer)) {
						gf_bs_read_int_log_idx(bs, state->buffer_removal_time_length, "buffer_removal_time", opNum);
					}
				}
			}
		}
	}

	if (frame_state->frame_type == AV1_SWITCH_FRAME || (frame_state->frame_type == AV1_KEY_FRAME && frame_state->show_frame)) {
		frame_state->refresh_frame_flags = AV1_ALL_FRAMES;
	}
	else {
		frame_state->refresh_frame_flags = gf_bs_read_int_log(bs, 8, "refresh_frame_flags");
	}
	if (!FrameIsIntra || frame_state->refresh_frame_flags != AV1_ALL_FRAMES) {
		if (error_resilient_mode && state->enable_order_hint) {
			u32 i = 0;
			for (i = 0; i < AV1_NUM_REF_FRAMES; i++) {
				u8 ref_order_hint = gf_bs_read_int_log_idx(bs, state->OrderHintBits, "ref_order_hint", i);
				if (ref_order_hint != state->RefOrderHint[i]) {
					state->RefValid[i] = 0;
				}
				state->RefOrderHint[i] = ref_order_hint;
			}
		}
	}

	u8 allow_intrabc = 0;
	if (frame_state->frame_type == AV1_KEY_FRAME) {
		av1_frame_size(bs, state, frame_size_override_flag);
		av1_render_size(bs);
		if (allow_screen_content_tools && state->UpscaledWidth == state->width) {
			allow_intrabc = gf_bs_read_int_log(bs, 1, "allow_intrabc");
		}
	}
	else {
		if (frame_state->frame_type == AV1_INTRA_ONLY_FRAME) {
			av1_frame_size(bs, state, frame_size_override_flag);
			av1_render_size(bs);
			if (allow_screen_content_tools && state->UpscaledWidth == state->width) {
				allow_intrabc = gf_bs_read_int_log(bs, 1, "allow_intrabc");
			}
		}
		else {
			u32 i = 0;
			Bool frame_refs_short_signaling = GF_FALSE;
			if (state->enable_order_hint) {
				frame_refs_short_signaling = gf_bs_read_int_log(bs, 1, "frame_refs_short_signaling");
				if (frame_refs_short_signaling) {
					u8 last_frame_idx = gf_bs_read_int_log(bs, 3, "last_frame_idx");
					u8 gold_frame_idx = gf_bs_read_int_log(bs, 3, "gold_frame_idx");
					av1_set_frame_refs(state, last_frame_idx, gold_frame_idx, ref_frame_idx);
				}
			}
			for (i = 0; i < AV1_REFS_PER_FRAME; i++) {
				if (!frame_refs_short_signaling)
					ref_frame_idx[i] = gf_bs_read_int_log_idx(bs, 3, "ref_frame_idx", i);

				if (state->frame_id_numbers_present_flag) {
					u32 n = state->delta_frame_id_length_minus_2 + 2;
					/*delta_frame_id_minus_1 =*/ gf_bs_read_int_log_idx(bs, n, "delta_frame_id_minus1", i);
					//DeltaFrameId = delta_frame_id_minus_1 + 1;
					//expectedFrameId[i] = ((current_frame_id + (1 << idLen) - DeltaFrameId) % (1 << idLen));
				}
			}
			if (frame_size_override_flag && !error_resilient_mode) {
				frame_size_with_refs(bs, state, frame_size_override_flag, ref_frame_idx);
			}
			else {
				av1_frame_size(bs, state, frame_size_override_flag);
				av1_render_size(bs);
			}
			frame_state->allow_high_precision_mv = 0;
			if (!force_integer_mv) {
				frame_state->allow_high_precision_mv = gf_bs_read_int_log(bs, 1, "allow_high_precision_mv");
			}

			read_interpolation_filter(bs);

			gf_bs_read_int_log(bs, 1, "is_motion_mode_switchable");
			if (!(error_resilient_mode || !state->enable_ref_frame_mvs)) {
				gf_bs_read_int_log(bs, 1, "use_ref_frame_mvs");
			}
		}
	}

	if (!FrameIsIntra) {
		u32 i;
		for (i = 0; i < AV1_REFS_PER_FRAME; i++) {
			u8 refFrame = AV1_LAST_FRAME + i;
			u8 ridx = ref_frame_idx[i];
			if (ridx >= 0) {
				u8 hint = state->RefOrderHint[ridx];
				state->OrderHints[refFrame] = hint;
				/*			if ( !enable_order_hint ) {
								RefFrameSignBias[ refFrame ] = 0;
							} else {
								RefFrameSignBias[ refFrame ] = get_relative_dist( hint, OrderHint) > 0;
							}
				*/
			}

		}
	}

	if (!(state->reduced_still_picture_header || disable_cdf_update))
		gf_bs_read_int_log(bs, 1, "disable_frame_end_update_cdf");

	if (primary_ref_frame == AV1_PRIMARY_REF_NONE) {
		//init_non_coeff_cdfs();
		av1_setup_past_independence(state);
	}
	else {
		//load_cdfs(ref_frame_idx[primary_ref_frame]);
		av1_load_previous(state, primary_ref_frame, ref_frame_idx);
	}

	av1_parse_tile_info(bs, state);
	//quantization_params( ):
	u8 base_q_idx = gf_bs_read_int_log(bs, 8, "base_q_idx");
	s32 DeltaQUDc = 0;
	s32 DeltaQUAc = 0;
	s32 DeltaQVDc = 0;
	s32 DeltaQVAc = 0;
	s32 DeltaQYDc = av1_delta_q(bs, "DeltaQYDc_coded", "DeltaQYDc");
	if (!state->config->monochrome) {
		u8 diff_uv_delta = 0;
		if (state->separate_uv_delta_q)
			diff_uv_delta = gf_bs_read_int_log(bs, 1, "diff_uv_delta");

		DeltaQUDc = av1_delta_q(bs, "DeltaQUDc_coded", "DeltaQUDc");
		DeltaQUAc = av1_delta_q(bs, "DeltaQUAc_coded", "DeltaQUAc");
		if (diff_uv_delta) {
			DeltaQVDc = av1_delta_q(bs, "DeltaQVDc_coded", "DeltaQVDc");
			DeltaQVAc = av1_delta_q(bs, "DeltaQVAc_coded", "DeltaQVAc");
		}
	}
	if (gf_bs_read_int_log(bs, 1, "using_qmatrix")) {
		gf_bs_read_int_log(bs, 4, "qm_y");
		gf_bs_read_int_log(bs, 4, "qm_u");
		if (!state->separate_uv_delta_q) {
			gf_bs_read_int_log(bs, 4, "qm_v");
		}
	}

	u8 seg_features_SEG_LVL_ALT_Q_enabled[8] = { 0,0,0,0,0,0,0,0 };
	s32 seg_features_SEG_LVL_ALT_Q[8] = { 0,0,0,0,0,0,0,0 };

	//segmentation_params( ):
	u8 segmentation_enabled = gf_bs_read_int_log(bs, 1, "segmentation_enabled");
	if (segmentation_enabled) {
		/*u8 segmentation_temporal_update = 0;*/
		u8 segmentation_update_data = 1;
		if (primary_ref_frame != AV1_PRIMARY_REF_NONE) {
			u8 segmentation_update_map = gf_bs_read_int_log(bs, 1, "segmentation_update_map");
			if (segmentation_update_map == 1)
				gf_bs_read_int_log(bs, 1, "segmentation_temporal_update");
			segmentation_update_data = gf_bs_read_int_log(bs, 1, "segmentation_update_data");
		}
		if (segmentation_update_data == 1) {
			u32 i, j;
			for (i = 0; i < 8/*=MAX_SEGMENTS*/; i++) {
				for (j = 0; j < 8 /*=SEG_LVL_MAX*/; j++) {
					if (/*feature_enabled = */gf_bs_read_int_log_idx2(bs, 1, "feature_enabled", i, j) == 1) {
						s32 val;
						u32 bitsToRead = Segmentation_Feature_Bits[j];
						//this is SEG_LVL_ALT_Q
						if (!j) seg_features_SEG_LVL_ALT_Q_enabled[i] = 1;

						if (Segmentation_Feature_Signed[j] == 1) {
							val = gf_bs_read_int_log_idx2(bs, 1 + bitsToRead, "signed_feature_value", i, j);
						}
						else {
							val = gf_bs_read_int_log_idx2(bs, bitsToRead, "feature_value", i, j);
						}
						if (!j) seg_features_SEG_LVL_ALT_Q[i] = val;
					}
				}
			}
			//ignore all init steps
		}

	}

	//delta_q_params():
	/*u8 delta_q_res = 0;*/
	u8 delta_q_present = 0;
	if (base_q_idx > 0) {
		delta_q_present = gf_bs_read_int_log(bs, 1, "delta_q_present");
	}
	if (delta_q_present) {
		gf_bs_read_int_log(bs, 2, "delta_q_res");
	}

	//delta_lf_params():
	u8 delta_lf_present = 0;
	/*u8 delta_lf_res = 0;
	u8 delta_lf_multi = 0;*/
	if (delta_q_present) {
		if (!allow_intrabc) {
			delta_lf_present = gf_bs_read_int_log(bs, 1, "delta_lf_present");
		}
		if (delta_lf_present) {
			gf_bs_read_int_log(bs, 2, "delta_lf_res");
			gf_bs_read_int_log(bs, 1, "delta_lf_multi");
		}
	}

	//init lossless stuff!
	u8 CodedLossless = 1;
	for (idx = 0; idx < 8; idx++) {
		u8 qindex = av1_get_qindex(GF_TRUE, idx, base_q_idx, delta_q_present, 0/*CurrentQIndex always ignored at this level of parsin*/, segmentation_enabled, seg_features_SEG_LVL_ALT_Q_enabled, seg_features_SEG_LVL_ALT_Q);
		Bool LosslessArray = (qindex == 0) && (DeltaQYDc == 0) && (DeltaQUAc == 0) && (DeltaQUDc == 0) && (DeltaQVAc == 0) && (DeltaQVDc == 0);
		if (!LosslessArray)
			CodedLossless = 0;
	}
	Bool AllLossless = CodedLossless && (state->width == state->UpscaledWidth);

	//loop_filter_params():
	if (!CodedLossless && !allow_intrabc) {
		u8 loop_filter_level_0 = gf_bs_read_int_log(bs, 6, "loop_filter_level_0");
		u8 loop_filter_level_1 = gf_bs_read_int_log(bs, 6, "loop_filter_level_1");
		if (!state->config->monochrome) {
			if (loop_filter_level_0 || loop_filter_level_1) {
				gf_bs_read_int_log(bs, 6, "loop_filter_level_2");
				gf_bs_read_int_log(bs, 6, "loop_filter_level_3");
			}
		}
		gf_bs_read_int_log(bs, 3, "loop_filter_sharpness");
		u8 loop_filter_delta_enabled = gf_bs_read_int_log(bs, 1, "loop_filter_delta_enabled");
		if (loop_filter_delta_enabled == 1) {
			u8 loop_filter_delta_update = gf_bs_read_int_log(bs, 1, "loop_filter_delta_update");
			if (loop_filter_delta_update) {
				u32 i;
				for (i = 0; i < 8/*TOTAL_REFS_PER_FRAME*/; i++) {
					u8 update_ref_delta = gf_bs_read_int_log_idx(bs, 1, "update_ref_delta", i);
					if (update_ref_delta == 1) {
						gf_bs_read_int_log_idx(bs, 1 + 6, "loop_filter_ref_deltas", i);
					}
				}
				for (i = 0; i < 2; i++) {
					u8 update_mode_delta = gf_bs_read_int_log_idx(bs, 1, "update_mode_delta", i);
					if (update_mode_delta) {
						gf_bs_read_int_log_idx(bs, 1 + 6, "loop_filter_mode_deltas", i);
					}
				}
			}
		}
	}
	//cdef_params( ):
	if (!CodedLossless && !allow_intrabc && state->enable_cdef) {
		gf_bs_read_int_log(bs, 2, "cdef_damping_minus_3");
		u8 cdef_bits = gf_bs_read_int_log(bs, 2, "cdef_bits");
		u32 i, num_cd = 1 << cdef_bits;
		for (i = 0; i < num_cd; i++) {
			gf_bs_read_int_log_idx(bs, 4, "cdef_y_pri_strength", i);
			gf_bs_read_int_log_idx(bs, 2, "cdef_y_sec_strength", i);
			if (!state->config->monochrome) {
				gf_bs_read_int_log_idx(bs, 4, "cdef_uv_pri_strength", i);
				gf_bs_read_int_log_idx(bs, 2, "cdef_uv_sec_strength", i);
			}
		}
	}

	//lr_params( ) :
	if (!AllLossless && !allow_intrabc && state->enable_restoration) {
		u32 i, nb_planes = state->config->monochrome ? 1 : 3;
		u8 UsesLr = 0;
		u8 usesChromaLr = 0;
		for (i = 0; i < nb_planes; i++) {
			u8 lr_type = gf_bs_read_int_log_idx(bs, 2, "lr_type", i);
			//FrameRestorationType[i] = Remap_Lr_Type[lr_type]
			if (lr_type != AV1_RESTORE_NONE) {
				UsesLr = 1;
				if (i > 0) {
					usesChromaLr = 1;
				}
			}
		}
		if (UsesLr) {
			if (state->use_128x128_superblock) {
				gf_bs_read_int_log(bs, 1, "lr_unit_shift_minus_1");
			}
			else {
				u8 lr_unit_shift = gf_bs_read_int_log(bs, 1, "lr_unit_shift");
				if (lr_unit_shift) {
					gf_bs_read_int_log(bs, 1, "lr_unit_extra_shift");
					//lr_unit_shift += lr_unit_extra_shift;
				}
			}
			if (state->config->chroma_subsampling_x && state->config->chroma_subsampling_y && usesChromaLr) {
				gf_bs_read_int_log(bs, 1, "lr_uv_shift");
			}
		}
	}
	//read_tx_mode():
	if (CodedLossless == 1) {
	}
	else {
		gf_bs_read_int_log(bs, 1, "tx_mode_select");
	}

	//frame_reference_mode( ):
	u8 reference_select = 0;
	if (FrameIsIntra) {
	}
	else {
		reference_select = gf_bs_read_int_log(bs, 1, "reference_select");
	}

	//skip_mode_params( ):
	u8 skipModeAllowed = 0;
	if (FrameIsIntra || !reference_select || !state->enable_order_hint) {
	}
	else {
		u32 i;
		s32 forwardIdx = -1;
		s32 backwardIdx = -1;
		s32 forwardHint = 0;
		s32 backwardHint = 0;
		for (i = 0; i < AV1_REFS_PER_FRAME; i++) {
			u8 refHint = state->RefOrderHint[ref_frame_idx[i]];
			if (av1_get_relative_dist(refHint, frame_state->order_hint, state) < 0) {
				if (forwardIdx < 0 || av1_get_relative_dist(refHint, forwardHint, state) > 0) {
					forwardIdx = i;
					forwardHint = refHint;
				}
			}
			else if (av1_get_relative_dist(refHint, frame_state->order_hint, state) > 0) {
				if (backwardIdx < 0 || av1_get_relative_dist(refHint, backwardHint, state) < 0) {
					backwardIdx = i;
					backwardHint = refHint;
				}
			}
		}
		if (forwardIdx < 0) {
			skipModeAllowed = 0;
		}
		else if (backwardIdx >= 0) {
			skipModeAllowed = 1;
			//SkipModeFrame[0] = AV1_LAST_FRAME + MIN(forwardIdx, backwardIdx);
			//SkipModeFrame[1] = AV1_LAST_FRAME + MAX(forwardIdx, backwardIdx);
		}
		else {
			s32 secondForwardIdx = -1;
			s32 secondForwardHint = 0;
			for (i = 0; i < AV1_REFS_PER_FRAME; i++) {
				u8 refHint = state->RefOrderHint[ref_frame_idx[i]];
				if (av1_get_relative_dist(refHint, forwardHint, state) < 0) {
					if (secondForwardIdx < 0 || av1_get_relative_dist(refHint, secondForwardHint, state) > 0) {
						secondForwardIdx = i;
						secondForwardHint = refHint;
					}
				}
			}
			if (secondForwardIdx < 0) {
				skipModeAllowed = 0;
			}
			else {
				skipModeAllowed = 1;
				//SkipModeFrame[ 0 ] = LAST_FRAME + Min(forwardIdx, secondForwardIdx)
				//SkipModeFrame[ 1 ] = LAST_FRAME + Max(forwardIdx, secondForwardIdx)
			}
		}
	}
	if (skipModeAllowed) {
		gf_bs_read_int_log(bs, 1, "skip_mode_present");
	}


	if (FrameIsIntra || error_resilient_mode || !state->enable_warped_motion) {

	}
	else {
		gf_bs_read_int_log(bs, 1, "allow_warped_motion");
	}

	gf_bs_read_int_log(bs, 1, "reduced_tx");

	//global_motion_params( )
	u32 ref;
	for (ref = AV1_LAST_FRAME; ref <= AV1_ALTREF_FRAME; ref++) {
		u32 i;
		for (i = 0; i < 6; i++) {
			state->GmParams.coefs[ref][i] = ((i % 3 == 2) ? 1 << WARPEDMODEL_PREC_BITS : 0);
		}
	}
	if (!FrameIsIntra) {
		u32 refs;
		for (refs = AV1_LAST_FRAME; refs <= AV1_ALTREF_FRAME; refs++) {
			u8 type = AV1_GMC_IDENTITY;
			Bool is_global = gf_bs_read_int_log_idx(bs, 1, "is_global", refs);
			if (is_global) {
				Bool is_rot_zoom = gf_bs_read_int_log_idx(bs, 1, "is_rot_zoom", refs);
				if (is_rot_zoom) {
					type = AV1_GMC_ROTZOOM;
				}
				else {
					Bool is_trans = gf_bs_read_int_log_idx(bs, 1, "is_translation", refs);
					type = is_trans ? AV1_GMC_TRANSLATION : AV1_GMC_AFFINE;

				}
			}

			if (type >= AV1_GMC_ROTZOOM) {
				av1_read_global_param(state, bs, type, refs, 2);
				av1_read_global_param(state, bs, type, refs, 3);
				if (type == AV1_GMC_AFFINE) {
					av1_read_global_param(state, bs, type, refs, 4);
					av1_read_global_param(state, bs, type, refs, 5);
				}
				else {
					state->GmParams.coefs[refs][4] = -state->GmParams.coefs[refs][3];
					state->GmParams.coefs[refs][5] = state->GmParams.coefs[refs][2];

				}
			}
			if (type >= AV1_GMC_TRANSLATION) {
				av1_read_global_param(state, bs, type, refs, 0);
				av1_read_global_param(state, bs, type, refs, 1);
			}
		}
	}

	//film_grain_params()
	if (!state->film_grain_params_present || (!state->frame_state.show_frame && !showable_frame)) {
	}
	else {
		u8 apply_grain = gf_bs_read_int_log(bs, 1, "apply_grain");
		if (apply_grain) {
			gf_bs_read_int_log(bs, 16, "grain_seed");
			u8 update_grain = 1;
			if (state->frame_state.frame_type == AV1_INTER_FRAME) {
				update_grain = gf_bs_read_int_log(bs, 1, "update_grain");
			}
			if (!update_grain) {
				gf_bs_read_int_log(bs, 3, "film_grain_params_ref_idx");
			}
			else {
				u32 i, num_y_points = gf_bs_read_int_log(bs, 4, "num_y_points");
				for (i = 0; i < num_y_points; i++) {
					gf_bs_read_int_log_idx(bs, 8, "point_y_value", i);
					gf_bs_read_int_log_idx(bs, 8, "point_y_scaling", i);
				}
				u8 chroma_scaling_from_luma = 0;
				if (!state->config->monochrome)
					chroma_scaling_from_luma = gf_bs_read_int_log(bs, 1, "chroma_scaling_from_luma");

				u8 num_cb_points = 0;
				u8 num_cr_points = 0;
				if (state->config->monochrome || chroma_scaling_from_luma ||
					((state->config->chroma_subsampling_x == 1) && (state->config->chroma_subsampling_y == 1) && (num_y_points == 0))
					) {
				}
				else {
					num_cb_points = gf_bs_read_int_log(bs, 4, "num_cb_points");
					for (i = 0; i < num_cb_points; i++) {
						gf_bs_read_int_log_idx(bs, 8, "point_cb_value", i);
						gf_bs_read_int_log_idx(bs, 8, "point_cb_scaling", i);
					}
					num_cr_points = gf_bs_read_int_log(bs, 4, "num_cr_points");
					for (i = 0; i < num_cr_points; i++) {
						gf_bs_read_int_log_idx(bs, 8, "point_cr_value", i);
						gf_bs_read_int_log_idx(bs, 8, "point_cr_scaling", i);
					}
				}
				gf_bs_read_int_log(bs, 2, "grain_scaling_minus_8");
				u8 ar_coeff_lag = gf_bs_read_int_log(bs, 2, "ar_coeff_lag");
				u16 numPosLuma = 2 * ar_coeff_lag * (ar_coeff_lag + 1);
				u16 numPosChroma = numPosLuma;
				if (num_y_points) {
					numPosChroma = numPosLuma + 1;
					for (i = 0; i < numPosLuma; i++) {
						gf_bs_read_int_log_idx(bs, 8, "ar_coeffs_y_plus_128", i);
					}
				}
				if (chroma_scaling_from_luma || num_cb_points) {
					for (i = 0; i < numPosChroma; i++) {
						gf_bs_read_int_log_idx(bs, 8, "ar_coeffs_cb_plus_128", i);
					}
				}
				if (chroma_scaling_from_luma || num_cr_points) {
					for (i = 0; i < numPosChroma; i++) {
						gf_bs_read_int_log_idx(bs, 8, "ar_coeffs_cr_plus_128", i);
					}
				}
				gf_bs_read_int_log(bs, 2, "ar_coeff_shift_minus_6");
				gf_bs_read_int_log(bs, 2, "grain_scale_shift");
				if (num_cb_points) {
					gf_bs_read_int_log(bs, 8, "cb_mult");
					gf_bs_read_int_log(bs, 8, "cb_luma_mult");
					gf_bs_read_int_log(bs, 9, "cb_offset");
				}
				if (num_cr_points) {
					gf_bs_read_int_log(bs, 8, "cr_mult");
					gf_bs_read_int_log(bs, 8, "cr_luma_mult");
					gf_bs_read_int_log(bs, 9, "cr_offset");
				}
				gf_bs_read_int_log(bs, 1, "overlap_flag");
				gf_bs_read_int_log(bs, 1, "clip_to_restricted_range");
			}
		}
	}

	//end of uncompressed header !!
}