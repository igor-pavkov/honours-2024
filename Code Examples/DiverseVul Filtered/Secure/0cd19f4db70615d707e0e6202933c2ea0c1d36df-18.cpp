static void vvc_parse_general_timing_hrd_parameters(GF_BitStream *bs, VVC_SPS *sps, VVC_VPS *vps, Bool *general_nal_hrd_params_present_flag, Bool *general_vcl_hrd_params_present_flag, Bool *general_du_hrd_params_present_flag, u32 *hrd_cpb_cnt_minus1)
{
	sps->has_timing_info = 1;
	sps->num_units_in_tick = gf_bs_read_int_log(bs, 32, "num_units_in_tick");
	sps->time_scale = gf_bs_read_int_log(bs, 32, "timescale");
	*general_du_hrd_params_present_flag = GF_FALSE;
	*general_nal_hrd_params_present_flag = gf_bs_read_int_log(bs, 1, "general_nal_hrd_params_present_flag");
	*general_vcl_hrd_params_present_flag = gf_bs_read_int_log(bs, 1, "general_vcl_hrd_params_present_flag");
	if (*general_nal_hrd_params_present_flag || *general_vcl_hrd_params_present_flag) {
		gf_bs_read_int_log(bs, 1, "general_same_pic_timing_in_all_ols_flag");
		*general_du_hrd_params_present_flag = gf_bs_read_int_log(bs, 1, "general_du_hrd_params_present_flag");
		if (*general_du_hrd_params_present_flag)
			gf_bs_read_int_log(bs, 8, "tick_divisor_minus2");
		gf_bs_read_int_log(bs, 4, "bit_rate_scale");
		gf_bs_read_int_log(bs, 4, "cpb_size_scale");
		if (*general_du_hrd_params_present_flag)
			gf_bs_read_int_log(bs, 4, "cpb_size_du_scale");
		*hrd_cpb_cnt_minus1 = gf_bs_read_ue_log(bs, "hrd_cpb_cnt_minus1");
	}
}