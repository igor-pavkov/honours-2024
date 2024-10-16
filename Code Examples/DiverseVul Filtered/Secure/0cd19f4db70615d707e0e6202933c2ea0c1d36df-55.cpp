s32 gf_vvc_parse_nalu_bs(GF_BitStream *bs, VVCState *vvc, u8 *nal_unit_type, u8 *temporal_id, u8 *layer_id)
{
	Bool is_slice = GF_FALSE;
	s32 ret = -1;
	Bool poc_reset = GF_FALSE;
	VVCSliceInfo n_state;

	gf_bs_enable_emulation_byte_removal(bs, GF_TRUE);
	if (gf_bs_available(bs)<2) return -1;
	gf_bs_mark_overflow(bs, GF_TRUE);

	memcpy(&n_state, &vvc->s_info, sizeof(VVCSliceInfo));
	if (!vvc_parse_nal_header(bs, nal_unit_type, temporal_id, layer_id)) return -1;

	n_state.nal_unit_type = *nal_unit_type;

	switch (n_state.nal_unit_type) {
	case GF_VVC_NALU_ACCESS_UNIT:
	case GF_VVC_NALU_END_OF_SEQ:
	case GF_VVC_NALU_END_OF_STREAM:
		//don't restore slice info, we don't have any change and n_state.poc_lsb / n_state.poc_msb is not valid
		vvc->s_info.nal_unit_type = n_state.nal_unit_type;
		return 1;

	case GF_VVC_NALU_SLICE_IDR_W_RADL:
	case GF_VVC_NALU_SLICE_IDR_N_LP:
		poc_reset = GF_TRUE;
	case GF_VVC_NALU_SLICE_TRAIL:
	case GF_VVC_NALU_SLICE_STSA:
	case GF_VVC_NALU_SLICE_RADL:
	case GF_VVC_NALU_SLICE_RASL:
	case GF_VVC_NALU_SLICE_CRA:
	case GF_VVC_NALU_SLICE_GDR:
		/* slice - read the info and compare.*/
		ret = vvc_parse_slice(bs, vvc, &n_state);
		if (ret < 0) {
			memcpy(&vvc->s_info, &n_state, sizeof(VVCSliceInfo));
			return ret;
		}
		
		ret = 0;
		if (n_state.compute_poc_defer || n_state.picture_header_in_slice_header_flag) {
			is_slice = GF_TRUE;
			n_state.compute_poc_defer = 0;

			vvc_compute_poc(&n_state, poc_reset);
			if (vvc->s_info.poc != n_state.poc) {
				ret = 1;
				break;
			}
			if (!(*layer_id) || (n_state.prev_layer_id_plus1 && ((*layer_id) <= n_state.prev_layer_id_plus1 - 1))) {
				ret = 1;
				break;
			}
		}
		break;

	case GF_VVC_NALU_PIC_HEADER:
		if (vvc_parse_picture_header(bs, vvc, &n_state)<0) {
			ret = -1;
			break;
		}
		is_slice = GF_TRUE;

		//we cannot compute poc until we know the first picture unit type, since IDR will reset poc count
		//and irap_or_gdr_pic=0 does not prevent IDR from following
		n_state.compute_poc_defer = 1;

		if (!(*layer_id) || (n_state.prev_layer_id_plus1 && ((*layer_id) <= n_state.prev_layer_id_plus1 - 1))) {
			ret = 1;
		}
		break;
	case GF_VVC_NALU_SEQ_PARAM:
		vvc->last_parsed_sps_id = gf_vvc_read_sps_bs_internal(bs, vvc, *layer_id, NULL);
		ret = (vvc->last_parsed_sps_id>=0) ? 0 : -1;
		break;
	case GF_VVC_NALU_PIC_PARAM:
		vvc->last_parsed_pps_id = gf_vvc_read_pps_bs_internal(bs, vvc);
		ret = (vvc->last_parsed_pps_id>=0) ? 0 : -1;
		break;
	case GF_VVC_NALU_VID_PARAM:
		vvc->last_parsed_vps_id = gf_vvc_read_vps_bs_internal(bs, vvc, GF_FALSE);
		ret = (vvc->last_parsed_vps_id>=0) ? 0 : -1;
		break;
	case GF_VVC_NALU_DEC_PARAM:
		ret = 0;
		break;
	case GF_VVC_NALU_APS_PREFIX:
		//we use the mix aps type + aps id (first 8 bits) as unique identifier
		vvc->last_parsed_aps_id = gf_bs_read_int_log(bs, 8, "aps_id");
		ret = 0;
		break;
	default:
		ret = 0;
		break;
	}
	if (gf_bs_is_overflow(bs)) ret = -1;

	/* save current POC lsb/msb to prev values */
	if ((ret>0) && vvc->s_info.sps) {
		if (!n_state.compute_poc_defer) {
			n_state.poc_lsb_prev = n_state.poc_lsb;
			n_state.poc_msb_prev = n_state.poc_msb;
		}
		if (is_slice)
			n_state.prev_layer_id_plus1 = *layer_id + 1;
	}

	memcpy(&vvc->s_info, &n_state, sizeof(VVCSliceInfo));

	return ret;
}