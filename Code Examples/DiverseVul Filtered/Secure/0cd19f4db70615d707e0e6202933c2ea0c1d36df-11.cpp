static void vvc_parse_ols_timing_hrd_parameters(GF_BitStream *bs, u32 firstSubLayer, u32 MaxSubLayersVal, Bool general_nal_hrd_params_present_flag, Bool general_vcl_hrd_params_present_flag, Bool general_du_hrd_params_present_flag, u32 hrd_cpb_cnt_minus1)
{
	u32 i;
	for (i=firstSubLayer; i<=MaxSubLayersVal; i++) {
		Bool fixed_pic_rate_within_cvs_flag = GF_TRUE;
		if (! gf_bs_read_int_log_idx(bs, 1, "fixed_pic_rate_general_flag", i)) {
			fixed_pic_rate_within_cvs_flag = gf_bs_read_int_log_idx(bs, 1, "fixed_pic_rate_within_cvs_flag", i);
		}
		if (fixed_pic_rate_within_cvs_flag) {
			gf_bs_read_ue_log(bs, "elemental_duration_in_tc_minus1");
		} else if ( (general_nal_hrd_params_present_flag || general_vcl_hrd_params_present_flag)
			&& (hrd_cpb_cnt_minus1 ==0)
		) {
			gf_bs_read_int_log_idx(bs, 1, "low_delay_hrd_flag", i);
		}
		if (general_nal_hrd_params_present_flag) {
			vvc_parse_sublayer_hrd_parameters(bs, i, general_du_hrd_params_present_flag, hrd_cpb_cnt_minus1);
		}
		if (general_vcl_hrd_params_present_flag) {
			vvc_parse_sublayer_hrd_parameters(bs, i, general_du_hrd_params_present_flag, hrd_cpb_cnt_minus1);
		}
	}
}