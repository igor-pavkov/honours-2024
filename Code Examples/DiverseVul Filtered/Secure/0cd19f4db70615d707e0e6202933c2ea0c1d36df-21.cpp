s32 gf_hevc_parse_nalu_bs(GF_BitStream *bs, HEVCState *hevc, u8 *nal_unit_type, u8 *temporal_id, u8 *layer_id)
{
	Bool is_slice = GF_FALSE;
	s32 ret = -1;
	HEVCSliceInfo n_state;

	gf_bs_enable_emulation_byte_removal(bs, GF_TRUE);
	if (gf_bs_available(bs)<2) return -1;

	gf_bs_mark_overflow(bs, GF_TRUE);

	memcpy(&n_state, &hevc->s_info, sizeof(HEVCSliceInfo));
	if (!hevc_parse_nal_header(bs, nal_unit_type, temporal_id, layer_id)) return -1;

	n_state.nal_unit_type = *nal_unit_type;

	switch (n_state.nal_unit_type) {
	case GF_HEVC_NALU_ACCESS_UNIT:
	case GF_HEVC_NALU_END_OF_SEQ:
	case GF_HEVC_NALU_END_OF_STREAM:
		ret = 1;
		break;

		/*slice_segment_layer_rbsp*/
	case GF_HEVC_NALU_SLICE_TRAIL_N:
	case GF_HEVC_NALU_SLICE_TRAIL_R:
	case GF_HEVC_NALU_SLICE_TSA_N:
	case GF_HEVC_NALU_SLICE_TSA_R:
	case GF_HEVC_NALU_SLICE_STSA_N:
	case GF_HEVC_NALU_SLICE_STSA_R:
	case GF_HEVC_NALU_SLICE_BLA_W_LP:
	case GF_HEVC_NALU_SLICE_BLA_W_DLP:
	case GF_HEVC_NALU_SLICE_BLA_N_LP:
	case GF_HEVC_NALU_SLICE_IDR_W_DLP:
	case GF_HEVC_NALU_SLICE_IDR_N_LP:
	case GF_HEVC_NALU_SLICE_CRA:
	case GF_HEVC_NALU_SLICE_RADL_N:
	case GF_HEVC_NALU_SLICE_RADL_R:
	case GF_HEVC_NALU_SLICE_RASL_N:
	case GF_HEVC_NALU_SLICE_RASL_R:
		is_slice = GF_TRUE;
		/* slice - read the info and compare.*/
		ret = hevc_parse_slice_segment(bs, hevc, &n_state);
		if (ret < 0) return ret;

		hevc_compute_poc(&n_state);

		ret = 0;

		if (hevc->s_info.poc != n_state.poc) {
			ret = 1;
			break;
		}
		if (n_state.first_slice_segment_in_pic_flag) {
			if (!(*layer_id) || (n_state.prev_layer_id_plus1 && ((*layer_id) <= n_state.prev_layer_id_plus1 - 1))) {
				ret = 1;
				break;
			}
		}
		break;
	case GF_HEVC_NALU_SEQ_PARAM:
		hevc->last_parsed_sps_id = gf_hevc_read_sps_bs_internal(bs, hevc, *layer_id, NULL);
		ret = (hevc->last_parsed_sps_id>=0) ? 0 : -1;
		break;
	case GF_HEVC_NALU_PIC_PARAM:
		hevc->last_parsed_pps_id = gf_hevc_read_pps_bs_internal(bs, hevc);
		ret = (hevc->last_parsed_pps_id>=0) ? 0 : -1;
		break;
	case GF_HEVC_NALU_VID_PARAM:
		hevc->last_parsed_vps_id = gf_hevc_read_vps_bs_internal(bs, hevc, GF_FALSE);
		ret = (hevc->last_parsed_vps_id>=0) ? 0 : -1;
		break;
	default:
		ret = 0;
		break;
	}

	if (gf_bs_is_overflow(bs))
		ret = -1;

	/* save _prev values */
	if ((ret>0) && hevc->s_info.sps) {
		n_state.frame_num_offset_prev = hevc->s_info.frame_num_offset;
		n_state.frame_num_prev = hevc->s_info.frame_num;

		n_state.poc_lsb_prev = hevc->s_info.poc_lsb;
		n_state.poc_msb_prev = hevc->s_info.poc_msb;
		if (is_slice)
			n_state.prev_layer_id_plus1 = *layer_id + 1;
	}
	if (is_slice) hevc_compute_poc(&n_state);
	memcpy(&hevc->s_info, &n_state, sizeof(HEVCSliceInfo));

	return ret;
}