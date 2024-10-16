static void vvc_profile_tier_level(GF_BitStream *bs, VVC_ProfileTierLevel *ptl, u32 idx)
{
	u32 i;
	if (ptl->pt_present) {
		ptl->general_profile_idc = gf_bs_read_int_log_idx(bs, 7, "general_profile_idc", idx);
		ptl->general_tier_flag = gf_bs_read_int_log_idx(bs, 1, "general_tier_flag", idx);
	}
	ptl->general_level_idc = gf_bs_read_int_log_idx(bs, 8, "general_level_idc", idx);
	ptl->frame_only_constraint = gf_bs_read_int_log_idx(bs, 1, "frame_only_constraint", idx);
	ptl->multilayer_enabled = gf_bs_read_int_log_idx(bs, 1, "multilayer_enabled", idx);
	//general constraints info - max size if 1 + 81 + 8 + 255
	if (ptl->pt_present) {
		//		general_constraints_info
		ptl->gci_present = gf_bs_read_int_log_idx(bs, 1, "gci_present", idx);
		if (ptl->gci_present) {
			u8 res;
			ptl->gci[0] = 0x80;
			ptl->gci[0] |= gf_bs_read_int(bs, 7);
			//71 buts till reserved, so 71-7 = 64bits = 8 bytes till reserved
			gf_bs_read_data(bs, ptl->gci+1, 8);
			ptl->gci[10] = gf_bs_read_int(bs, 2)<<6;
			//skip extensions
			ptl->gci[11] = 0;
			res = gf_bs_read_int(bs, 8);
			gf_bs_read_int(bs, res);
		}
		gf_bs_align(bs);
	}
	for (i=ptl->ptl_max_tid; i>0; i--) {
		ptl->sub_ptl[i-1].level_present_flag = gf_bs_read_int_log_idx2(bs, 1, "level_present_flag", idx, i);
	}
	gf_bs_align(bs);
	for (i=ptl->ptl_max_tid; i>0; i--) {
		if (ptl->sub_ptl[i-1].level_present_flag)
			ptl->sub_ptl[i-1].sublayer_level_idc = gf_bs_read_int_log_idx2(bs, 8, "sublayer_level_idc", idx, i);
	}
	if (ptl->pt_present) {
		ptl->num_sub_profiles = gf_bs_read_int_log_idx(bs, 8, "num_sub_profiles", idx);
		for (i=0; i<ptl->num_sub_profiles; i++) {
			ptl->sub_profile_idc[i] = gf_bs_read_int_log_idx2(bs, 32, "sub_profile_idc", idx, i);
		}
	}
}