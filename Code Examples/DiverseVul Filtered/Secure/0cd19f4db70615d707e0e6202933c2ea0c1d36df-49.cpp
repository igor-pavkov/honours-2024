static s32 vvc_parse_ref_pic_list_struct(GF_BitStream *bs, VVC_SPS *sps, u32 listIdx, u32 rplsIdx, VVC_RefPicList *rpl)
{
	u32 i;
	memset(rpl, 0, sizeof(VVC_RefPicList));
	rpl->num_ref_entries = gf_bs_read_ue_log_idx2(bs, "num_ref_entries", listIdx, rplsIdx);
	if (rpl->num_ref_entries>=VVC_MAX_REF_PICS) {
		GF_LOG(GF_LOG_ERROR, GF_LOG_CODING, ("[VVC] num_ref_entries %d exceeds maximum allowed value %d\n", rpl->num_ref_entries, VVC_MAX_REF_PICS));
		return -1;
	}

	rpl->ltrp_in_header_flag = 1;
	if (sps->long_term_ref_pics_flag
		&& rplsIdx < sps->num_ref_pic_lists[listIdx]
		&& (rpl->num_ref_entries > 0)
	) {
		rpl->ltrp_in_header_flag = gf_bs_read_int_log_idx2(bs, 1, "ltrp_in_header_flag", listIdx, rplsIdx);
	}
	for (i=0; i < rpl->num_ref_entries; i++) {
		Bool inter_layer_ref_pic_flag = 0;
		if (sps->inter_layer_prediction_enabled_flag) {
			inter_layer_ref_pic_flag = gf_bs_read_int_log_idx3(bs, 1, "inter_layer_ref_pic_flag", listIdx, rplsIdx, i);
		}
		if (!inter_layer_ref_pic_flag) {
			u32 AbsDeltaPocSt;
			Bool st_ref_pic_flag = 1;
			if (sps->long_term_ref_pics_flag) {
				st_ref_pic_flag = gf_bs_read_int_log_idx3(bs, 1, "st_ref_pic_flag", listIdx, rplsIdx, i);
			}
			if (st_ref_pic_flag) {
				u32 abs_delta_poc_st = gf_bs_read_ue_log_idx3(bs, "abs_delta_poc_st", listIdx, rplsIdx, i);

				if ((sps->weighted_pred_flag || sps->weighted_bipred_flag) && (i!=0)) {
					AbsDeltaPocSt = abs_delta_poc_st;
				} else {
					AbsDeltaPocSt = abs_delta_poc_st + 1;
				}
				if (AbsDeltaPocSt>0) {
					gf_bs_read_int_log_idx3(bs, 1, "strp_entry_sign_flag", listIdx, rplsIdx, i);
				}
				rpl->nb_short_term_pictures++;
				rpl->ref_pic_type[i] = VVC_RPL_ST;
			} else if( !rpl->ltrp_in_header_flag) {
				gf_bs_read_int_log_idx3(bs, sps->log2_max_poc_lsb, "rpls_poc_lsb_lt", listIdx, rplsIdx, i);
				rpl->nb_long_term_pictures++;
				rpl->ref_pic_type[i] = VVC_RPL_LT;
			}
		} else {
			gf_bs_read_ue_log_idx3(bs, "ilrp_idx", listIdx, rplsIdx, i);
			rpl->nb_inter_layer_pictures ++;
			rpl->ref_pic_type[i] = VVC_RPL_IL;
		}
	}
	return 0;
}