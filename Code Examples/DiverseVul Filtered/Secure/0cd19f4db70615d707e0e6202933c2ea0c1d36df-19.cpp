GF_Err gf_vp9_parse_sample(GF_BitStream *bs, GF_VPConfig *vp9_cfg, Bool *key_frame, u32 *FrameWidth, u32 *FrameHeight, u32 *renderWidth, u32 *renderHeight)
{
	Bool FrameIsIntra = GF_FALSE, profile_low_bit, profile_high_bit, show_existing_frame = GF_FALSE, frame_type = GF_FALSE, show_frame = GF_FALSE, error_resilient_mode = GF_FALSE;
	/*u8 frame_context_idx = 0, reset_frame_context = 0, frame_marker = 0*/;
	int Sb64Cols = 0, Sb64Rows = 0, i;
	u8 refresh_frame_flags = 0;

	assert(bs && key_frame);

	/*uncompressed header*/
	/*frame_marker = */gf_bs_read_int_log(bs, 2, "frame_marker");
	profile_low_bit = gf_bs_read_int_log(bs, 1, "profile_low_bit");
	profile_high_bit = gf_bs_read_int_log(bs, 1, "profile_high_bit");
	vp9_cfg->profile = (profile_high_bit << 1) + profile_low_bit;
	if (vp9_cfg->profile == 3) {
		Bool reserved_zero = gf_bs_read_int_log(bs, 1, "reserved_zero");
		if (reserved_zero) {
			GF_LOG(GF_LOG_ERROR, GF_LOG_CODING, ("[VP9] uncompressed header reserved zero is not zero.\n"));
			return GF_NON_COMPLIANT_BITSTREAM;
		}
	}

	show_existing_frame = gf_bs_read_int_log(bs, 1, "show_existing_frame");
	if (show_existing_frame == GF_TRUE) {
		/*frame_to_show_map_idx = */gf_bs_read_int_log(bs, 3, "frame_to_show_map_idx");
		return GF_OK;
	}

	frame_type = gf_bs_read_int_log(bs, 1, "frame_type");
	show_frame = gf_bs_read_int_log(bs, 1, "show_frame");
	error_resilient_mode = gf_bs_read_int_log(bs, 1, "error_resilient_mode");
	if (frame_type == VP9_KEY_FRAME) {
		if (!vp9_frame_sync_code(bs))
			return GF_NON_COMPLIANT_BITSTREAM;
		if (vp9_color_config(bs, vp9_cfg) != GF_OK)
			return GF_NON_COMPLIANT_BITSTREAM;
		vp9_frame_size(bs, FrameWidth, FrameHeight, &Sb64Cols, &Sb64Rows);
		vp9_render_size(bs, *FrameWidth, *FrameHeight, renderWidth, renderHeight);
		refresh_frame_flags = 0xFF;
		*key_frame = GF_TRUE;
		FrameIsIntra = GF_TRUE;
	}
	else {
		Bool intra_only = GF_FALSE;
		*key_frame = GF_FALSE;

		if (show_frame == GF_FALSE) {
			intra_only = gf_bs_read_int_log(bs, 1, "intra_only");
		}
		FrameIsIntra = intra_only;

		if (error_resilient_mode == GF_FALSE) {
			/*reset_frame_context = */gf_bs_read_int_log(bs, 2, "reset_frame_context");
		}

		if (intra_only == GF_TRUE) {
			if (!vp9_frame_sync_code(bs))
				return GF_NON_COMPLIANT_BITSTREAM;

			if (vp9_cfg->profile > 0) {
				if (vp9_color_config(bs, vp9_cfg) != GF_OK)
					return GF_NON_COMPLIANT_BITSTREAM;
			}
			else {
				u8 color_space = CS_BT_601;
				vp9_cfg->colour_primaries = VP9_CS_to_23001_8_colour_primaries[color_space];
				vp9_cfg->transfer_characteristics = VP9_CS_to_23001_8_transfer_characteristics[color_space];
				vp9_cfg->matrix_coefficients = VP9_CS_to_23001_8_matrix_coefficients[color_space];
				vp9_cfg->chroma_subsampling = 0;
				vp9_cfg->bit_depth = 8;
			}
			refresh_frame_flags = gf_bs_read_int_log(bs, 8, "refresh_frame_flags");
			vp9_frame_size(bs, FrameWidth, FrameHeight, &Sb64Cols, &Sb64Rows);
			vp9_render_size(bs, *FrameWidth, *FrameHeight, renderWidth, renderHeight);
		}
		else {
			refresh_frame_flags = gf_bs_read_int_log(bs, 8, "refresh_frame_flags");
			u8 ref_frame_idx[3];
			for (i = 0; i < 3; i++) {
				ref_frame_idx[i] = gf_bs_read_int_log_idx(bs, 3, "ref_frame_idx", i);
				/*ref_frame_sign_bias[LAST_FRAME + i] = */gf_bs_read_int_log_idx(bs, 1, "ref_frame_sign_bias", i);
			}
			vp9_frame_size_with_refs(bs, refresh_frame_flags, ref_frame_idx, vp9_cfg->RefFrameWidth, vp9_cfg->RefFrameHeight, FrameWidth, FrameHeight, renderWidth, renderHeight, &Sb64Cols, &Sb64Rows);
			/*allow_high_precision_mv = */gf_bs_read_int_log(bs, 1, "allow_high_precision_mv");
			vp9_read_interpolation_filter(bs);
		}
	}

	if (error_resilient_mode == 0) {
		/*refresh_frame_context = */gf_bs_read_int_log(bs, 1, "refresh_frame_context");
		/*frame_parallel_decoding_mode = */gf_bs_read_int_log(bs, 1, "frame_parallel_decoding_mode");
	}

	/*frame_context_idx = */gf_bs_read_int_log(bs, 2, "frame_context_idx");
	if (FrameIsIntra || error_resilient_mode) {
		/*setup_past_independence + save_probs ...*/
		//frame_context_idx = 0;
	}

	vp9_loop_filter_params(bs);
	vp9_quantization_params(bs);
	vp9_segmentation_params(bs);
	vp9_tile_info(bs, Sb64Cols);

	/*header_size_in_bytes = */gf_bs_read_int_log(bs, 16, "header_size_in_bytes");

	/*Reference frame update process (8.10 - partial)*/
	for (i = 0; i < VP9_NUM_REF_FRAMES; i++) {
		if ((refresh_frame_flags >> i) & 1) {
			vp9_cfg->RefFrameWidth[i] = *FrameWidth;
			vp9_cfg->RefFrameHeight[i] = *FrameHeight;
		}
	}

	return GF_OK;
}