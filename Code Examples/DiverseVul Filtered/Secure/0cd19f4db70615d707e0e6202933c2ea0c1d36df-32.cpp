static void vvc_parse_sublayer_hrd_parameters(GF_BitStream *bs, u32 subLayerId, Bool general_du_hrd_params_present_flag, u32 hrd_cpb_cnt_minus1)
{
	u32 j;
	for (j=0; j <= hrd_cpb_cnt_minus1; j++) {
		gf_bs_read_ue_log_idx2(bs, "bit_rate_value_minus1", subLayerId, j);
		gf_bs_read_ue_log_idx2(bs, "cpb_size_value_minus1", subLayerId, j);
		if( general_du_hrd_params_present_flag ) {
			gf_bs_read_ue_log_idx2(bs, "cpb_size_du_value_minus1", subLayerId, j);
			gf_bs_read_ue_log_idx2(bs, "bit_rate_du_value_minus1", subLayerId, j);
		}
		gf_bs_read_int_log_idx2(bs, 1, "cbr_flag", subLayerId, j);
	}
}