static void avc_pred_weight_table(GF_BitStream *bs, u32 slice_type, u32 ChromaArrayType, u32 num_ref_idx_l0_active_minus1, u32 num_ref_idx_l1_active_minus1) {
	u32 i, j;
	gf_bs_read_ue_log(bs, "luma_log2_weight_denom");
	if (ChromaArrayType != 0) {
		gf_bs_read_ue_log(bs, "chroma_log2_weight_denom");
	}
	for (i = 0; i <= num_ref_idx_l0_active_minus1; i++) {
		if (gf_bs_read_int_log_idx(bs, 1, "luma_weight_l0_flag", i)) {
			gf_bs_read_se_log_idx(bs, "luma_weight_l0", i);
			gf_bs_read_se_log_idx(bs, "luma_offset_l0", i);
		}
		if (ChromaArrayType != 0) {
			if (gf_bs_read_int_log_idx(bs, 1, "chroma_weight_l0_flag", i))
				for (j = 0; j < 2; j++) {
					gf_bs_read_se_log_idx2(bs, "chroma_weight_l0", i, j);
					gf_bs_read_se_log_idx2(bs, "chroma_offset_l0", i, j);
				}
		}
	}
	if (slice_type % 5 == 1) {
		for (i = 0; i <= num_ref_idx_l1_active_minus1; i++) {
			if (gf_bs_read_int_log_idx(bs, 1, "luma_weight_l1_flag", i)) {
				gf_bs_read_se_log_idx(bs, "luma_weight_l1", i);
				gf_bs_read_se_log_idx(bs, "luma_offset_l1", i);
			}
			if (ChromaArrayType != 0) {
				if (gf_bs_read_int_log_idx(bs, 1, "chroma_weight_l1_flag", i)) {
					for (j = 0; j < 2; j++) {
						gf_bs_read_se_log_idx2(bs, "chroma_weight_l1", i, j);
						gf_bs_read_se_log_idx2(bs, "chroma_offset_l1", i, j);
					}
				}
			}
		}
	}
}