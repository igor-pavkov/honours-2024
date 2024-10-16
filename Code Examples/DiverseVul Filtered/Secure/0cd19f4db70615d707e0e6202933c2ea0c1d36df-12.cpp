static s32 vvc_parse_ref_pic_lists(GF_BitStream *bs, VVCSliceInfo *si, Bool is_pic_header)
{
	u32 i;
	s32 *p_rpl_idx = is_pic_header ? &si->ph_rpl_idx[0] : &si->rpl_idx[0];

	u8 rpl_sps_flag_prev=0;
	for (i=0; i<2; i++) {
		VVC_RefPicList *rpl=NULL;
		u32 j;
		u8 rpl_sps_flag=0;
		u32 rpl_idx = 0;
		if ((si->sps->num_ref_pic_lists[i]>0) && (!i || si->pps->rpl1_idx_present_flag)) {
			rpl_sps_flag = gf_bs_read_int_log_idx(bs, 1, "rpl_sps_flag", i);
		}
		/*
		When rpl_sps_flag[ i ] is not present, it is inferred as follows:
		⎯ If sps_num_ref_pic_lists[ i ] is equal to 0, the value of rpl_sps_flag[ i ] is inferred to be equal to 0.
		⎯ Otherwise (sps_num_ref_pic_lists[ i ] is greater than 0), when pps_rpl1_idx_present_flag is equal to 0 and i is equal to 1, the value of rpl_sps_flag[ 1 ] is inferred to be equal to rpl_sps_flag[ 0 ].
		*/
		else {
			if (si->sps->num_ref_pic_lists[i]==0) {
				rpl_sps_flag = 0;
			} else {
				rpl_sps_flag = rpl_sps_flag_prev;
			}
		}
		rpl_sps_flag_prev = rpl_sps_flag;
		if (is_pic_header) {
			rpl = &si->ph_rpl[i];
		} else {
			rpl = &si->rpl[i];
		}

		if (rpl_sps_flag) {
			if ((si->sps->num_ref_pic_lists[i]>1) && (!i || si->pps->rpl1_idx_present_flag)) {
				u32 nb_bits =  gf_get_bit_size(si->sps->num_ref_pic_lists[i]);
				rpl_idx = gf_bs_read_int_log_idx(bs, nb_bits, "rpl_idx", i);
			}
			else if (si->sps->num_ref_pic_lists[i] == 1) {
				rpl_idx = 0;
			} else {
				assert(p_rpl_idx[0] != -1);
				rpl_idx = p_rpl_idx[0];
			}
			p_rpl_idx[i] = rpl_idx;
			if (rpl_idx>=VVC_MAX_REF_PICS) {
				GF_LOG(GF_LOG_WARNING, GF_LOG_CODING, ("[VVC] Picture header RplIdx %d greater than max allowed %d\n", rpl_idx, VVC_MAX_REF_PICS));
				return -1;
			}

			memcpy(rpl, &si->sps->rps[i][rpl_idx], sizeof(VVC_RefPicList));

		} else {
			s32 res = vvc_parse_ref_pic_list_struct(bs, si->sps, i, si->sps->num_ref_pic_lists[i], rpl);
			if (res<0) return res;
			p_rpl_idx[i] = -1;
		}

		if (rpl->nb_long_term_pictures) {
			for (j=0; j<rpl->num_ref_entries; j++) {
				if (rpl->ref_pic_type[j] != VVC_RPL_LT) continue;

				if (rpl->ltrp_in_header_flag) {
					gf_bs_read_int_log_idx2(bs, si->sps->log2_max_poc_lsb, "poc_lsb_lt", i, j);
				}
				if (gf_bs_read_int_log_idx2(bs, 1, "delta_poc_msb_cycle_present_flag", i, j)) {
					gf_bs_read_ue_log_idx2(bs, "delta_poc_msb_cycle_lt", i, j);
				}
			}
		}
	}
	return 0;
}