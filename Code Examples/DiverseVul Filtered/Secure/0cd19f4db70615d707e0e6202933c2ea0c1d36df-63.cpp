static s32 avc_parse_hrd_parameters(GF_BitStream *bs, AVC_HRD *hrd)
{
	int i, cpb_cnt_minus1;

	cpb_cnt_minus1 = gf_bs_read_ue_log(bs, "cpb_cnt_minus1");
	if (cpb_cnt_minus1 > 31) {
		GF_LOG(GF_LOG_WARNING, GF_LOG_CODING, ("[avc-h264] invalid cpb_cnt_minus1 value: %d (expected in [0;31])\n", cpb_cnt_minus1));
		return -1;
	}
	gf_bs_read_int_log(bs, 4, "bit_rate_scale");
	gf_bs_read_int_log(bs, 4, "cpb_size_scale");

	/*for( SchedSelIdx = 0; SchedSelIdx <= cpb_cnt_minus1; SchedSelIdx++ ) {*/
	for (i = 0; i <= cpb_cnt_minus1; i++) {
		gf_bs_read_ue_log_idx(bs, "bit_rate_value_minus1", i);
		gf_bs_read_ue_log_idx(bs, "cpb_size_value_minus1", i);
		gf_bs_read_int_log_idx(bs, 1, "cbr_flag", i);
	}
	gf_bs_read_int_log(bs, 5, "initial_cpb_removal_delay_length_minus1");
	hrd->cpb_removal_delay_length_minus1 = gf_bs_read_int_log(bs, 5, "cpb_removal_delay_length_minus1");
	hrd->dpb_output_delay_length_minus1 = gf_bs_read_int_log(bs, 5, "dpb_output_delay_length_minus1");
	hrd->time_offset_length = gf_bs_read_int_log(bs, 5, "time_offset_length");
	return 0;
}