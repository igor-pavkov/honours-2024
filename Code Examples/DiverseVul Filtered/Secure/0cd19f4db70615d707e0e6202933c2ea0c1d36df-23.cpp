static s32 vvc_parse_picture_header(GF_BitStream *bs, VVCState *vvc, VVCSliceInfo *si)
{
	s32 pps_id;

	si->irap_or_gdr_pic = gf_bs_read_int_log(bs, 1, "irap_or_gdr_pic");
	si->non_ref_pic = gf_bs_read_int_log(bs, 1, "non_ref_pic");
	if (si->irap_or_gdr_pic)
		si->gdr_pic = gf_bs_read_int_log(bs, 1, "gdr_pic");

	si->intra_slice_allowed_flag = 1;
	if ((si->inter_slice_allowed_flag = gf_bs_read_int_log(bs, 1, "inter_slice_allowed_flag")))
		si->intra_slice_allowed_flag = gf_bs_read_int_log(bs, 1, "intra_slice_allowed_flag");

	pps_id = gf_bs_read_ue_log(bs, "pps_id");
	if ((pps_id<0) || (pps_id >= 64))
		return -1;
	si->pps = &vvc->pps[pps_id];
	si->sps = &vvc->sps[si->pps->sps_id];
	si->poc_lsb = gf_bs_read_int_log(bs, si->sps->log2_max_poc_lsb, "poc_lsb");

	si->recovery_point_valid = 0;
	si->gdr_recovery_count = 0;
	if (si->gdr_pic) {
		si->recovery_point_valid = 1;
		si->gdr_recovery_count = gf_bs_read_ue_log(bs, "gdr_recovery_count");
	}
	gf_bs_read_int_log(bs, si->sps->ph_num_extra_bits, "ph_extra_bits");

	if (si->sps->poc_msb_cycle_flag) {
		if ( (si->poc_msb_cycle_present_flag = gf_bs_read_int_log(bs, 1, "poc_msb_cycle_present_flag"))) {
			si->poc_msb_cycle = gf_bs_read_int_log(bs, si->sps->poc_msb_cycle_len, "poc_msb_cycle");
		}
	}

	if (si->sps->alf_enabled_flag && si->pps->alf_info_in_ph_flag ) {
		if (gf_bs_read_int_log(bs, 1, "ph_alf_enabled_flag")) {
			u32 i, nb_aps_id = gf_bs_read_int_log(bs, 3, "ph_num_alf_aps_ids_luma");
			for (i=0; i<nb_aps_id; i++) {
				gf_bs_read_int_log_idx(bs, 3, "ph_alf_aps_id_luma", i);
			}
			u8 alf_cb_enabled_flag = 0, alf_cr_enabled_flag=0;
			if (si->sps->chroma_format_idc) {
				alf_cb_enabled_flag = gf_bs_read_int_log(bs, 1, "ph_alf_cb_enabled_flag");
				alf_cr_enabled_flag = gf_bs_read_int_log(bs, 1, "ph_alf_cr_enabled_flag");
			}
			if (alf_cb_enabled_flag || alf_cr_enabled_flag) {
				gf_bs_read_int_log(bs, 3, "ph_alf_aps_id_chroma");
			}
			if (si->sps->ccalf_enabled_flag ) {
				if (gf_bs_read_int_log(bs, 1, "ph_alf_cc_cb_enabled_flag")) {
					gf_bs_read_int_log(bs, 3, "ph_alf_cc_cb_aps_id");
				}
				if (gf_bs_read_int_log(bs, 1, "ph_alf_cc_cr_enabled_flag")) {
					gf_bs_read_int_log(bs, 3, "ph_alf_cc_cr_aps_id");
				}
			}
		}
	}
	si->lmcs_enabled_flag = 0;
	if (si->sps->lmcs_enabled_flag) {
		si->lmcs_enabled_flag = gf_bs_read_int_log(bs, 1, "ph_lmcs_enabled_flag");
		if (si->lmcs_enabled_flag) {
			gf_bs_read_int_log(bs, 2, "ph_lmcs_aps_id");
			if (si->sps->chroma_format_idc) {
				gf_bs_read_int_log(bs, 1, "ph_chroma_residual_scale_flag");
			}
		}
	}
	si->explicit_scaling_list_enabled_flag = 0;
	if (si->sps->explicit_scaling_list_enabled_flag) {
		si->explicit_scaling_list_enabled_flag = gf_bs_read_int_log(bs, 1, "ph_explicit_scaling_list_enabled_flag");
		if (si->explicit_scaling_list_enabled_flag) {
			gf_bs_read_int_log(bs, 3, "ph_scaling_list_aps_id");
		}
	}
	if (si->sps->virtual_boundaries_enabled_flag && !si->sps->virtual_boundaries_present_flag) {
		if (gf_bs_read_int_log(bs, 1, "ph_virtual_boundaries_present_flag")) {
			u32 i, nb_virt_boundaries = gf_bs_read_ue_log(bs, "ph_num_ver_virtual_boundaries");
			for (i=0; i<nb_virt_boundaries; i++) {
				gf_bs_read_ue_log_idx(bs, "ph_virtual_boundary_pos_x_minus1", i);
			}
			nb_virt_boundaries = gf_bs_read_ue_log(bs, "ph_num_hor_virtual_boundaries");
			for (i=0; i<nb_virt_boundaries; i++) {
				gf_bs_read_ue_log_idx(bs, "ph_virtual_boundary_pos_x_minus1", i);
			}
		}
	}
	if (si->pps->output_flag_present_flag && !si->non_ref_pic) {
		gf_bs_read_int_log(bs, 1, "ph_pic_output_flag");
	}
	if (si->pps->rpl_info_in_ph_flag) {
		s32 res = vvc_parse_ref_pic_lists(bs, si, GF_TRUE);
		if (res<0) return res;
	}
	u8 partition_constraints_override_flag = 0;
	if (si->sps->partition_constraints_override_enabled_flag) {
		partition_constraints_override_flag = gf_bs_read_int_log(bs, 1, "ph_partition_constraints_override_flag");
	}
	if (si->intra_slice_allowed_flag) {
		if (partition_constraints_override_flag) {
			gf_bs_read_ue_log(bs, "ph_log2_diff_min_qt_min_cb_inter_slice");
			u32 max_mtt_hierarchy_depth_inter_slice = gf_bs_read_ue_log(bs, "ph_max_mtt_hierarchy_depth_inter_slice");
			if (max_mtt_hierarchy_depth_inter_slice) {
				gf_bs_read_ue_log(bs, "ph_log2_diff_max_bt_min_qt_inter_slice");
				gf_bs_read_ue_log(bs, "ph_log2_diff_max_tt_min_qt_inter_slice");
			}
		}
		if (si->pps->cu_qp_delta_enabled_flag) {
			gf_bs_read_ue_log(bs, "ph_cu_qp_delta_subdiv_inter_slice");
		}
		if (si->pps->cu_chroma_qp_offset_list_enabled_flag) {
			gf_bs_read_ue_log(bs, "ph_cu_chroma_qp_offset_subdiv_intra_slice");
		}
	}
	si->temporal_mvp_enabled_flag = 0;
	if (si->inter_slice_allowed_flag) {
		if (partition_constraints_override_flag) {
			gf_bs_read_ue_log(bs, "ph_log2_diff_min_qt_min_cb_inter_slice");
			u32 max_mtt_hierarchy_depth_inter_slice = gf_bs_read_ue_log(bs, "ph_max_mtt_hierarchy_depth_inter_slice");
			if (max_mtt_hierarchy_depth_inter_slice) {
				gf_bs_read_ue_log(bs, "ph_log2_diff_max_bt_min_qt_inter_slice");
				gf_bs_read_ue_log(bs, "ph_log2_diff_max_tt_min_qt_inter_slice");
			}
		}
		if (si->pps->cu_qp_delta_enabled_flag) {
			gf_bs_read_ue_log(bs, "ph_cu_qp_delta_subdiv_inter_slice");
		}
		if (si->pps->cu_chroma_qp_offset_list_enabled_flag) {
			gf_bs_read_ue_log(bs, "ph_cu_chroma_qp_offset_subdiv_inter_slice");
		}
		if (si->sps->temporal_mvp_enabled_flag) {
			si->temporal_mvp_enabled_flag = gf_bs_read_int_log(bs, 1, "ph_temporal_mvp_enabled_flag");
			if (si->temporal_mvp_enabled_flag && si->pps->rpl_info_in_ph_flag) {
				u8 collocated_from_l0_flag = 1;
				if (si->ph_rpl[1].num_ref_entries>0)
					collocated_from_l0_flag = gf_bs_read_int_log(bs, 1, "ph_collocated_from_l0_flag");

				if ( (collocated_from_l0_flag && si->ph_rpl[0].num_ref_entries > 1)
					|| (!collocated_from_l0_flag && si->ph_rpl[1].num_ref_entries > 1)
				) {
					gf_bs_read_ue_log(bs, "ph_collocated_ref_idx");
				}
			}
		}
		if (si->sps->mmvd_fullpel_only_enabled_flag) {
			gf_bs_read_int_log(bs, 1, "ph_mmvd_fullpel_only_flag");
		}
		u8 presenceFlag = 0;
		if (!si->pps->rpl_info_in_ph_flag) {
			presenceFlag = 1;
		}
		else if (si->ph_rpl[1].num_ref_entries > 0) {
			presenceFlag = 1;
		}
		if (presenceFlag) {
			gf_bs_read_int_log(bs, 1, "ph_mvd_l1_zero_flag");
			if (si->sps->bdof_control_present_in_ph_flag)
				gf_bs_read_int_log(bs, 1, "ph_bdof_disabled_flag");
			if (si->sps->dmvr_control_present_in_ph_flag)
				gf_bs_read_int_log(bs, 1, "ph_dmvr_disabled_flag");
		}
		if (si->sps->prof_control_present_in_ph_flag)
			gf_bs_read_int_log(bs, 1, "ph_prof_disabled_flag");

		if ( (si->pps->weighted_pred_flag || si->pps->weighted_bipred_flag) && si->pps->wp_info_in_ph_flag) {
			s32 res = vvc_pred_weight_table(bs, vvc, si, si->pps, si->sps, NULL);
			if (res<0) return res;
		}
	}
	if (si->pps->qp_delta_info_in_ph_flag) {
		gf_bs_read_se_log(bs, "ph_qp_delta");
	}
	if (si->sps->joint_cbcr_enabled_flag) {
		gf_bs_read_int_log(bs, 1, "ph_joint_cbcr_sign_flag");
	}
	if (si->sps->sao_enabled_flag && si->pps->sao_info_in_ph_flag) {
		gf_bs_read_int_log(bs, 1, "ph_sao_luma_enabled_flag");
		if (si->sps->chroma_format_idc)
			gf_bs_read_int_log(bs, 1, "ph_sao_chroma_enabled_flag");
	}
	if (si->pps->dbf_info_in_ph_flag) {
		if (gf_bs_read_int_log(bs, 1, "ph_deblocking_params_present_flag")) {
			//defaults to 0
			u8 deblocking_filter_disabled_flag = 0;

			if (!si->pps->deblocking_filter_disabled_flag) {
				deblocking_filter_disabled_flag = gf_bs_read_int_log(bs, 1, "deblocking_filter_disabled_flag");
			}
			if (!deblocking_filter_disabled_flag) {
				gf_bs_read_se_log(bs, "ph_luma_beta_offset_div2");
				gf_bs_read_se_log(bs, "ph_luma_tc_offset_div2");
				if (si->pps->chroma_tool_offsets_present_flag) {
					gf_bs_read_se_log(bs, "ph_cb_beta_offset_div2");
					gf_bs_read_se_log(bs, "ph_cb_tc_offset_div2");
					gf_bs_read_se_log(bs, "ph_cr_beta_offset_div2");
					gf_bs_read_se_log(bs, "ph_cr_tc_offset_div2");
				}
			}
		}
	}
	if (si->pps->picture_header_extension_present_flag) {
		u32 i=0, num_ext = gf_bs_read_ue_log(bs, "ph_extension_length");
		while (i<num_ext) {
			gf_bs_read_int_log_idx(bs, 8, "ph_extension_data_byte", i);
			i++;
		}
	}
	return 0;
}