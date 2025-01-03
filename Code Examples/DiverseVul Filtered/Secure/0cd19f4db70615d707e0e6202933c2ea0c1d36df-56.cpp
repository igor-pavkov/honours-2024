static s32 vvc_parse_slice(GF_BitStream *bs, VVCState *vvc, VVCSliceInfo *si)
{
	u32 i, slice_address=0, num_tiles_in_slice=1;

	si->picture_header_in_slice_header_flag = gf_bs_read_int_log(bs, 1, "picture_header_in_slice_header_flag");
	if (si->picture_header_in_slice_header_flag) {
		s32 res = vvc_parse_picture_header(bs, vvc, si);
		if (res<0) return res;
	}
	if (!si->sps) return -1;
	if (!si->pps) return -1;
	si->slice_type = GF_VVC_SLICE_TYPE_I;
	if (si->sps->subpic_info_present) {
		gf_bs_read_int_log(bs, si->sps->subpicid_len, "subpic_id");
	}

	if (si->pps->rect_slice_flag) {
		if ((si->sps->nb_subpics==1) && (si->pps->num_slices_in_pic<=1)) {

		} else {
			if (vvc->parse_mode==1) {
				GF_LOG(GF_LOG_WARNING, GF_LOG_CODING, ("[VVC] sub-picture parsing not yet implemented, wrong slice header size estimation (result might be non-compliant) - patch welcome\n"));

				gf_bs_align(bs);
				si->payload_start_offset = (u32) gf_bs_get_position(bs);
				return -2;
			}
			if (vvc->parse_mode) {
				GF_LOG(GF_LOG_WARNING, GF_LOG_CODING, ("[VVC] sub-picture parsing not supported, aborting slice header parsing - patch welcome\n"));
			}
			return 0;
			//TODO
	// update CurrSubpicIdx
	//		if (NumSlicesInSubpic[CurrSubpicIdx] > 1) {
	//			sh_slice_address
	//		}
		}
	} else {
		if (si->pps->num_tiles_in_pic > 1) {
			slice_address = gf_bs_read_int_log(bs, si->pps->slice_address_len, "sh_slice_address");
		}
	}

	gf_bs_read_int_log(bs, si->sps->sh_num_extra_bits, "num_extra_bits");

	if (!si->pps->rect_slice_flag && (si->pps->num_tiles_in_pic - slice_address > 1)) {
		num_tiles_in_slice = 1 + gf_bs_read_ue_log(bs, "sh_num_tiles_in_slice_minus1");
	}

	if (si->inter_slice_allowed_flag )
		si->slice_type = gf_bs_read_ue_log(bs, "slice_type");

	if (!vvc->parse_mode) return 0;

	switch (si->nal_unit_type) {
	case GF_VVC_NALU_SLICE_IDR_W_RADL:
	case GF_VVC_NALU_SLICE_IDR_N_LP:
	case GF_VVC_NALU_SLICE_CRA:
	case GF_VVC_NALU_SLICE_GDR:
		gf_bs_read_int_log(bs, 1, "sh_no_output_of_prior_pics_flag");
		break;
	}
	if (si->sps->alf_enabled_flag && !si->pps->alf_info_in_ph_flag) {
		if (gf_bs_read_int_log(bs, 1, "sh_alf_enabled_flag")) {
			u32 nb_vals = gf_bs_read_int_log(bs, 3, "sh_num_alf_aps_ids_luma");
			for (i=0; i<nb_vals; i++) {
				gf_bs_read_int_log(bs, 3, "sh_alf_aps_id_luma");
			}
			u8 sh_alf_cb_enabled_flag = 0;
			u8 sh_alf_cr_enabled_flag = 0;
			if (si->sps->chroma_format_idc) {
				sh_alf_cb_enabled_flag = gf_bs_read_int_log(bs, 1, "sh_alf_cb_enabled_flag");
				sh_alf_cr_enabled_flag = gf_bs_read_int_log(bs, 1, "sh_alf_cr_enabled_flag");
			}
			if (sh_alf_cb_enabled_flag || sh_alf_cr_enabled_flag) {
				gf_bs_read_int_log(bs, 3, "sh_alf_aps_id_chroma");
			}
			if (si->sps->ccalf_enabled_flag) {
				if (gf_bs_read_int_log(bs, 1, "sh_alf_cc_cb_enabled_flag")) {
					gf_bs_read_int_log(bs, 3, "sh_alf_cc_cb_aps_id");
				}
				if (gf_bs_read_int_log(bs, 1, "sh_alf_cc_cr_enabled_flag")) {
					gf_bs_read_int_log(bs, 3, "sh_alf_cc_cr_aps_id");
				}
			}
		}
	}
	if (si->lmcs_enabled_flag && !si->picture_header_in_slice_header_flag) {
		gf_bs_read_int_log(bs, 1, "sh_lmcs_used_flag");
	}
	if (si->explicit_scaling_list_enabled_flag && !si->picture_header_in_slice_header_flag) {
		gf_bs_read_int_log(bs, 3, "sh_explicit_scaling_list_used_flag");
	}

	if (si->pps->rpl_info_in_ph_flag) {
		si->rpl[0] = si->ph_rpl[0];
		si->rpl[1] = si->ph_rpl[1];
	} else {
		memset(&si->rpl[0], 0, sizeof(VVC_RefPicList));
		memset(&si->rpl[1], 0, sizeof(VVC_RefPicList));
	}

	if (!si->pps->rpl_info_in_ph_flag
		&& ( ( (si->nal_unit_type != GF_VVC_NALU_SLICE_IDR_N_LP) && (si->nal_unit_type != GF_VVC_NALU_SLICE_IDR_W_RADL) ) || si->sps->idr_rpl_present_flag)
	) {
		s32 res = vvc_parse_ref_pic_lists(bs, si, GF_FALSE);
		if (res<0) return (vvc->parse_mode==1) ? res : 0;
	}

	u32 num_ref_idx_active[2] = {0, 0};

	if (
		((si->slice_type != GF_VVC_SLICE_TYPE_I) && (si->rpl[0].num_ref_entries > 1) )
		|| ((si->slice_type == GF_VVC_SLICE_TYPE_B) && (si->rpl[1].num_ref_entries > 1))
	) {
		if (gf_bs_read_int_log(bs, 1, "sh_num_ref_idx_active_override_flag")) {
			//L0
			u32 nb_active = 0;
			if (si->rpl[0].num_ref_entries>1) {
				nb_active = 1 + gf_bs_read_ue_log_idx(bs, "sh_num_ref_idx_active_minus1", 0);
			}
			num_ref_idx_active[0] = nb_active;
			//L1
			if (si->slice_type == GF_VVC_SLICE_TYPE_B) {
				nb_active = 0;
				if (si->rpl[1].num_ref_entries>1) {
					nb_active = 1 + gf_bs_read_ue_log_idx(bs, "sh_num_ref_idx_active_minus1", 1);
				}
				num_ref_idx_active[1] = nb_active;
			} else {
				num_ref_idx_active[1] = 0;
			}
		} else {
			if (si->rpl[0].num_ref_entries >= si->pps->num_ref_idx_default_active[0]) {
				num_ref_idx_active[0] = si->pps->num_ref_idx_default_active[0];
			} else {
				num_ref_idx_active[0] = si->rpl[0].num_ref_entries;
			}
			if (si->slice_type == GF_VVC_SLICE_TYPE_B) {
				if (si->rpl[1].num_ref_entries >= si->pps->num_ref_idx_default_active[1]) {
					num_ref_idx_active[1] = si->pps->num_ref_idx_default_active[1];
				} else {
					num_ref_idx_active[1] = si->rpl[1].num_ref_entries;
				}
			} else {
				num_ref_idx_active[1] = 0;
			}
		}
	} else {
        num_ref_idx_active[0] = (si->slice_type == GF_VVC_SLICE_TYPE_I) ? 0 : 1;
        num_ref_idx_active[1] = (si->slice_type == GF_VVC_SLICE_TYPE_B) ? 1 : 0;
	}

	if (si->slice_type != GF_VVC_SLICE_TYPE_I) {
		if (si->pps->cabac_init_present_flag)
			gf_bs_read_int_log(bs, 1, "sh_cabac_init_flag");

		if (si->temporal_mvp_enabled_flag && !si->pps->rpl_info_in_ph_flag) {
			u8 collocated_from_l0_flag = 0;
			if (si->slice_type == GF_VVC_SLICE_TYPE_B) {
				collocated_from_l0_flag = gf_bs_read_int_log(bs, 1, "sh_collocated_from_l0_flag");
			}
			if ( (collocated_from_l0_flag && (num_ref_idx_active[0] > 1))
				|| (!collocated_from_l0_flag && (num_ref_idx_active[1] > 1))
			) {
				gf_bs_read_ue_log(bs, "sh_collocated_ref_idx");
			}
		}
		if (!si->pps->wp_info_in_ph_flag
			&& (
				(si->pps->weighted_pred_flag && (si->slice_type == GF_VVC_SLICE_TYPE_P) )
				|| (si->pps->weighted_bipred_flag && (si->slice_type == GF_VVC_SLICE_TYPE_B))
			)
		) {
			s32 res = vvc_pred_weight_table(bs, vvc, si, si->pps, si->sps, num_ref_idx_active);
			if (res<0) return (vvc->parse_mode==1) ? res : 0;
		}
	}
	if (!si->pps->qp_delta_info_in_ph_flag) {
		gf_bs_read_se_log(bs, "sh_qp_delta");
	}
	if (si->pps->slice_chroma_qp_offsets_present_flag) {
		gf_bs_read_se_log(bs, "sh_cb_qp_offset");
		gf_bs_read_se_log(bs, "sh_cr_qp_offset");
		if (si->sps->joint_cbcr_enabled_flag)
			gf_bs_read_se_log(bs, "sh_joint_cbcr_qp_offset");
	}
	if (si->pps->cu_chroma_qp_offset_list_enabled_flag)
		gf_bs_read_int_log(bs, 1, "sh_cu_chroma_qp_offset_enabled_flag");
	if (si->sps->sao_enabled_flag && !si->pps->sao_info_in_ph_flag) {
		gf_bs_read_int_log(bs, 1, "sh_sao_luma_used_flag");
		if (si->sps->chroma_format_idc)
			gf_bs_read_int_log(bs, 1, "sh_sao_chroma_used_flag");
	}

	if (si->pps->deblocking_filter_override_enabled_flag && !si->pps->dbf_info_in_ph_flag) {
		if (gf_bs_read_int_log(bs, 1, "sh_deblocking_params_present_flag")) {
			u8 deblocking_params_present_flag=0;
			if (!si->pps->deblocking_filter_disabled_flag) {
				deblocking_params_present_flag = gf_bs_read_int_log(bs, 1, "sh_deblocking_filter_disabled_flag");
			}
			if (deblocking_params_present_flag) {
				gf_bs_read_se_log(bs, "sh_luma_beta_offset_div2");
				gf_bs_read_se_log(bs, "sh_luma_tc_offset_div2");
				if (si->pps->chroma_tool_offsets_present_flag) {
					gf_bs_read_se_log(bs, "sh_cb_beta_offset_div2");
					gf_bs_read_se_log(bs, "sh_cb_tc_offset_div2");
					gf_bs_read_se_log(bs, "sh_cr_beta_offset_div2");
					gf_bs_read_se_log(bs, "sh_cr_tc_offset_div2");
				}
			}
		}
	}

	u8 dep_quant_used_flag = 0;
	if (si->sps->dep_quant_enabled_flag) {
		dep_quant_used_flag = gf_bs_read_int_log(bs, 1, "sh_dep_quant_used_flag");
	}
	u8 sign_data_hiding_used_flag = 0;
	if (si->sps->sign_data_hiding_enabled_flag && !dep_quant_used_flag) {
		sign_data_hiding_used_flag = gf_bs_read_int_log(bs, 1, "sh_sign_data_hiding_used_flag");
	}
	u8 ts_residual_coding_disabled_flag = 0;
	if (si->sps->transform_skip_enabled_flag && !dep_quant_used_flag && !sign_data_hiding_used_flag) {
		ts_residual_coding_disabled_flag = gf_bs_read_int_log(bs, 1, "sh_ts_residual_coding_disabled_flag");
	}
	if (!ts_residual_coding_disabled_flag && si->sps->ts_residual_coding_rice_present_in_sh_flag) {
		gf_bs_read_int_log(bs, 3, "sh_ts_residual_coding_rice_idx_minus1");
	}
	if (si->sps->reverse_last_sig_coeff_enabled_flag) {
		gf_bs_read_int_log(bs, 1, "sh_reverse_last_sig_coeff_flag");
	}
	if (si->pps->slice_header_extension_present_flag) {
		u32 j=0, slice_header_extension_length = gf_bs_read_ue_log(bs, "sh_slice_header_extension_length");
		while (j<slice_header_extension_length) {
			gf_bs_read_int_log_idx(bs, 8, "sh_slice_header_extension_data_byte", j);
			j++;
		}
	}

	if (!si->pps->rect_slice_flag && !si->pps->num_tile_cols && !si->pps->num_tile_rows) {
	} else {
		if (si->sps->entry_point_offsets_present_flag && si->pps->rect_slice_flag) {
			if (vvc->parse_mode==1) {
				GF_LOG(GF_LOG_WARNING, GF_LOG_CODING, ("[VVC] Entry point offsets parsing for sub-picture not yet implemented, wrong slice header size estimation (result might be non-compliant) - patch welcome\n"));

				gf_bs_align(bs);
				si->payload_start_offset = (u32) gf_bs_get_position(bs);
				return -2;
			}
			GF_LOG(GF_LOG_INFO, GF_LOG_CODING, ("[VVC] Entry point offsets parsing for sub-picture not yet implemented, aborting slice header parsing - patch welcome\n"));
			return 0;
		}

		u32 nb_entry_points = vvc_get_num_entry_points(si, slice_address, num_tiles_in_slice);
		if (nb_entry_points) {
			u32 nb_bits = 1 + gf_bs_read_ue_log(bs, "sh_entry_offset_len_minus1");
			for (i=0; i<nb_entry_points; i++) {
				gf_bs_read_int_log_idx(bs, nb_bits, "sh_entry_point_offset_minus1", i);
			}
		}
	}

	u8 align_bit = gf_bs_read_int(bs, 1);
	if (align_bit != 1) {
		GF_LOG(GF_LOG_WARNING, GF_LOG_CODING, ("[VVC] Align bit at end of slice header not set to 1 !\n"));
		//return error only for strict mdoe
		if (vvc->parse_mode==1) {
			return -1;
		}
	}
	gf_bs_align(bs);
	//end of slice header
	si->payload_start_offset = (u32) gf_bs_get_position(bs);
	return 0;
}