static s32 vvc_pred_weight_table(GF_BitStream *bs, VVCState *vvc, VVCSliceInfo *si, VVC_PPS *pps, VVC_SPS *sps, u32 *num_ref_idx_active)
{
	u32 i, num_weights;
	u8 weights[VVC_MAX_REF_PICS];
	gf_bs_read_ue_log(bs, "luma_log2_weight_denom");
	if (sps->chroma_format_idc) {
		gf_bs_read_se_log(bs, "delta_chroma_log2_weight_denom");
	}
	if (pps->wp_info_in_ph_flag) {
		num_weights = gf_bs_read_ue_log(bs, "num_l0_weights");
	} else {
		num_weights = num_ref_idx_active[0];
	}
	if (num_weights>VVC_MAX_REF_PICS) {
		GF_LOG(GF_LOG_ERROR, GF_LOG_CODING, ("[VVC] num weights L0 %d greater than max allowed %d\n", num_weights, VVC_MAX_REF_PICS));
		return -1;
	}

	memset(weights, 0, sizeof(u8)*VVC_MAX_REF_PICS);
	for (i=0; i<num_weights; i++) {
		if (gf_bs_read_int_log_idx(bs, 1, "luma_weight_l0_flag", i))
			weights[i] |= 1;
	}
	if (sps->chroma_format_idc) {
		for (i=0; i<num_weights; i++) {
			if (gf_bs_read_int_log_idx(bs, 1, "chroma_weight_l0_flag", i))
				weights[i] |= 2;
		}
	}
	for (i=0; i<num_weights; i++) {
		if (weights[i] & 1) {
			gf_bs_read_se_log_idx(bs, "delta_luma_weight_l0", i);
			gf_bs_read_se_log_idx(bs, "luma_offset_l0", i);
		}
		if (weights[i] & 2) {
			gf_bs_read_se_log_idx2(bs, "delta_chroma_weight_l0", i, 0);
			gf_bs_read_se_log_idx2(bs, "delta_chroma_offset_l0", i, 0);
			gf_bs_read_se_log_idx2(bs, "delta_chroma_weight_l0", i, 1);
			gf_bs_read_se_log_idx2(bs, "delta_chroma_offset_l0", i, 1);
		}
	}
	num_weights = 0;
	if (pps->weighted_bipred_flag && pps->wp_info_in_ph_flag && (si->ph_rpl[1].num_ref_entries > 0)) {
		num_weights = gf_bs_read_ue_log(bs, "num_l1_weights");
	}
	if (!num_weights) return 0;

	if (num_weights>VVC_MAX_REF_PICS) {
		GF_LOG(GF_LOG_ERROR, GF_LOG_CODING, ("[VVC] num weights L1 %d greater than max allowed %d\n", num_weights, VVC_MAX_REF_PICS));
		return -1;
	}

	memset(weights, 0, sizeof(u8)*VVC_MAX_REF_PICS);
	for (i=0; i<num_weights; i++) {
		if (gf_bs_read_int_log_idx(bs, 1, "luma_weight_l1_flag", i))
			weights[i] |= 1;
	}
	if (sps->chroma_format_idc) {
		for (i=0; i<num_weights; i++) {
			if (gf_bs_read_int_log_idx(bs, 1, "chroma_weight_l1_flag", i))
				weights[i] |= 2;
		}
	}
	for (i=0; i<num_weights; i++) {
		if (weights[i] & 1) {
			gf_bs_read_se_log_idx(bs, "delta_luma_weight_l1", i);
			gf_bs_read_se_log_idx(bs, "luma_offset_l1", i);
		}
		if (weights[i] & 2) {
			gf_bs_read_se_log_idx2(bs, "delta_chroma_weight_l1", i, 0);
			gf_bs_read_se_log_idx2(bs, "delta_chroma_offset_l1", i, 0);
			gf_bs_read_se_log_idx2(bs, "delta_chroma_weight_l1", i, 1);
			gf_bs_read_se_log_idx2(bs, "delta_chroma_offset_l1", i, 1);
		}
	}
	return 0;
}