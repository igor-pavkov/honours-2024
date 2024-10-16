static void avc_hevc_vvc_rewrite_vui(GF_VUIInfo *vui_info, GF_BitStream *orig, GF_BitStream *mod, Bool is_vvc)
{
	/* VUI present flag*/
	Bool vui_present_flag = gf_bs_read_int(orig, 1);

	/*setup default values*/
	Bool aspect_ratio_info_present_flag = 0;
	s32 aspect_ratio_idc = -1;
	u32 ar_n=0, ar_d=0;
	Bool overscan_info_present_flag = 0;
	u32 overscan_info=0;
	u32 video_signal_type_present_flag=0;
	u32 video_format = 5;
	u32 video_full_range_flag = 0;
	u32 colour_description_present_flag = 0;
	u32 colour_primaries = 2;
	u32 transfer_characteristics = 2;
	u32 matrix_coefficients = 2;
	//VVC
	Bool progressive_source_flag = 1;
	Bool interlaced_source_flag = 0;
	Bool non_packed_constraint_flag = 0;
	Bool non_projected_constraint_flag = 0;
	Bool aspect_ratio_constant_flag = 1;
	u32 vui_start_pos = 0;
	u32 orig_vvc_payload_size = 0;
	Bool vui_chroma_loc_info_present_flag=0;
	u32 chroma_loc1=0, chroma_loc2=0;
	u32 final_vvc_payload_size = 8; //4 first bits + 4 flags (ar, overscan and colour desc, chroma pos)
	u32 mod_vui_start_pos = 0;

	//if VUI is present, read all SAR and overscan values
	if (vui_present_flag) { /* VUI found in input bitstream */
		if (is_vvc) {
			//align
			orig_vvc_payload_size = 8 * ( 1 + gf_bs_read_ue(orig) );
			gf_bs_align(orig);
			vui_start_pos = gf_bs_get_bit_offset(orig);

			progressive_source_flag = gf_bs_read_int(orig, 1);
			interlaced_source_flag = gf_bs_read_int(orig, 1);
			non_packed_constraint_flag = gf_bs_read_int(orig, 1);
			non_projected_constraint_flag = gf_bs_read_int(orig, 1);
		}
		aspect_ratio_info_present_flag = gf_bs_read_int(orig, 1);

		if (aspect_ratio_info_present_flag) {
			if (is_vvc) {
				aspect_ratio_constant_flag = gf_bs_read_int(orig, 1);
			}
			aspect_ratio_idc = gf_bs_read_int(orig, 8); /*aspect_ratio_idc*/
			if (aspect_ratio_idc == 255) {
				ar_n = gf_bs_read_int(orig, 16); /*sar_width*/
				ar_d = gf_bs_read_int(orig, 16); /*sar_height*/
			}
		}

		/*overscan_info_present_flag */
		overscan_info_present_flag = gf_bs_read_int(orig, 1);
		if(overscan_info_present_flag) {
			overscan_info = gf_bs_read_int(orig, 1);
		}

		/* read all video signal related flags first */
		video_signal_type_present_flag = gf_bs_read_int(orig, 1);

		if (video_signal_type_present_flag) {
			if (!is_vvc) {
				video_format = gf_bs_read_int(orig, 3);
				video_full_range_flag = gf_bs_read_int(orig, 1);
				colour_description_present_flag = gf_bs_read_int(orig, 1);
			} else {
				colour_description_present_flag = 1;
			}

			if (colour_description_present_flag) {
				colour_primaries = gf_bs_read_int(orig, 8);
				transfer_characteristics = gf_bs_read_int(orig, 8);
				matrix_coefficients = gf_bs_read_int(orig, 8);
				if (is_vvc) {
					video_full_range_flag = gf_bs_read_int(orig, 1);
				}
			}
		}
		if (is_vvc) {
			vui_chroma_loc_info_present_flag = gf_bs_read_int(orig, 1);
			if (vui_chroma_loc_info_present_flag) {
				if (progressive_source_flag && !interlaced_source_flag) {
					chroma_loc1 = gf_bs_read_ue(orig);
				} else {
					chroma_loc1 = gf_bs_read_ue(orig);
					chroma_loc2 = gf_bs_read_ue(orig);
				}
			}
		}
		//not VVC: don't read the rest
	}

	//recompute values
	//no change
	if ((vui_info->ar_num<0) || (vui_info->ar_den<0)) {
	}
	//remove par
	else if ((vui_info->ar_num==0) || (vui_info->ar_den==0)) {
		aspect_ratio_info_present_flag = 0;
	}
	//set par
	else {
		aspect_ratio_info_present_flag = 1;
	}

	//add par size
	if (aspect_ratio_info_present_flag) {
		ar_n = vui_info->ar_num;
		ar_d = vui_info->ar_den;
		aspect_ratio_idc = avc_hevc_get_sar_idx((u32) ar_n, (u32) ar_d);
		if (is_vvc) {
			final_vvc_payload_size += 9;
			if (aspect_ratio_idc==0xFF)
				final_vvc_payload_size += 32;
		}
	}

	if (vui_info->remove_video_info) {
		video_signal_type_present_flag = 0;
	}
	/* correct the values of each flags */
	else if ((vui_info->fullrange==0) && (vui_info->video_format==5) && (vui_info->color_prim==2) && (vui_info->color_tfc==2) && (vui_info->color_matrix==2)) {
		video_signal_type_present_flag = 0; /* all default, nothing to write*/
	} else {
		video_signal_type_present_flag = 1;
		video_format = (vui_info->video_format < 0) ? video_format : vui_info->video_format;
		video_full_range_flag = (vui_info->fullrange < 0) ? video_full_range_flag : vui_info->fullrange;
		if ((vui_info->color_prim==2) && (vui_info->color_tfc==2) && (vui_info->color_matrix==2)) {
			colour_description_present_flag = 0;
		} else {
			colour_description_present_flag = 1;
			colour_primaries = (vui_info->color_prim < 0) ? colour_primaries : vui_info->color_prim;
			transfer_characteristics = (vui_info->color_tfc < 0) ? transfer_characteristics : vui_info->color_tfc;
			matrix_coefficients = (vui_info->color_matrix < 0) ? matrix_coefficients : vui_info->color_matrix;
		}
		if ((colour_primaries==2) && (transfer_characteristics==2) && (matrix_coefficients==2)) {
			colour_description_present_flag = 0;
			if ((video_format==5) && (video_full_range_flag==0))
				video_signal_type_present_flag = 0;
		}

		if (is_vvc) {
			if (!video_full_range_flag && !colour_description_present_flag) {
				video_signal_type_present_flag = 0;
			} else {
				final_vvc_payload_size += 25;
			}
		}
	}

	if (is_vvc && vui_chroma_loc_info_present_flag) {
		if (progressive_source_flag && !interlaced_source_flag) {
			final_vvc_payload_size += gf_get_ue_nb_bits(chroma_loc1);
		} else {
			final_vvc_payload_size += gf_get_ue_nb_bits(chroma_loc1);
			final_vvc_payload_size += gf_get_ue_nb_bits(chroma_loc2);
		}
	}

	//always rewrite VUI
	gf_bs_write_int(mod, 1, 1);
	if (is_vvc) {
		while (final_vvc_payload_size%8)
			final_vvc_payload_size++;
		final_vvc_payload_size/=8;

		gf_bs_write_ue(mod, final_vvc_payload_size-1);
		gf_bs_align(mod);
		mod_vui_start_pos = gf_bs_get_bit_offset(mod);
		final_vvc_payload_size *= 8;

		gf_bs_write_int(mod, progressive_source_flag, 1);
		gf_bs_write_int(mod, interlaced_source_flag, 1);
		gf_bs_write_int(mod, non_packed_constraint_flag, 1);
		gf_bs_write_int(mod, non_projected_constraint_flag, 1);
	}

	gf_bs_write_int(mod, aspect_ratio_info_present_flag, 1);
	if (aspect_ratio_info_present_flag) {
		if (is_vvc)
			gf_bs_write_int(mod, aspect_ratio_constant_flag, 1);

		gf_bs_write_int(mod, aspect_ratio_idc, 8);
		if (aspect_ratio_idc == 255) {
			gf_bs_write_int(mod, ar_n, 16);
			gf_bs_write_int(mod, ar_d, 16);
		}
		if (vui_info->update) {
			vui_info->ar_num = ar_n;
			vui_info->ar_den = ar_d;
		}
	}
	gf_bs_write_int(mod, overscan_info_present_flag, 1);
	if (overscan_info_present_flag) {
		gf_bs_write_int(mod, overscan_info, 1);
	}

	gf_bs_write_int(mod, video_signal_type_present_flag, 1);
	if (video_signal_type_present_flag) {
		if (!is_vvc) {
			gf_bs_write_int(mod, video_format, 3);
			gf_bs_write_int(mod, video_full_range_flag, 1);
			gf_bs_write_int(mod, colour_description_present_flag, 1);
		} else {
			colour_description_present_flag = 1;
		}

		if (colour_description_present_flag) {
			gf_bs_write_int(mod, colour_primaries, 8);
			gf_bs_write_int(mod, transfer_characteristics, 8);
			gf_bs_write_int(mod, matrix_coefficients, 8);
			if (is_vvc)
				gf_bs_write_int(mod, video_full_range_flag, 1);
		}

		if (vui_info->update) {
			vui_info->video_format = video_format;
			vui_info->fullrange = video_full_range_flag;
			if (colour_description_present_flag) {
				vui_info->color_prim = colour_primaries;
				vui_info->color_tfc = transfer_characteristics;
				vui_info->color_matrix = matrix_coefficients;
			}
		}
	}

	if (is_vvc) {
		//write vui_chroma_loc_info_present_flag
		gf_bs_write_int(mod, vui_chroma_loc_info_present_flag, 1);
		if (vui_chroma_loc_info_present_flag) {
			if (progressive_source_flag && !interlaced_source_flag) {
				gf_bs_write_ue(mod, chroma_loc1);
			} else {
				gf_bs_write_ue(mod, chroma_loc1);
				gf_bs_write_ue(mod, chroma_loc2);
			}
		}
		//we don't copy over vui extension (they're not supposed to be present), but we must parse them
		if (vui_present_flag) {
			//we are byte aligned for vui_paramaters
			Bool more_data_in_payload = GF_TRUE;
			vui_start_pos = gf_bs_get_bit_offset(orig) - vui_start_pos;
			if (gf_bs_is_align(orig) && (vui_start_pos == orig_vvc_payload_size))
				more_data_in_payload = GF_FALSE;

			u32 nb_bits = orig_vvc_payload_size - vui_start_pos;
			if (more_data_in_payload) {
				if (nb_bits<8) {
					u32 val = gf_bs_peek_bits(orig, nb_bits, 0);
					u32 bit_pos = 1<<(nb_bits-1);
					if (val == bit_pos)
						more_data_in_payload = 0;
				}
			}
			if (more_data_in_payload) {
				while (nb_bits) {
					nb_bits--;
					gf_bs_read_int(orig, 1); //vui_reserved_payload_extension_data
					//load next 32 bits, if only 1 at MSB and 0 afterwards, done
					if (nb_bits<32) {
						u32 val = gf_bs_peek_bits(orig, nb_bits, 0);
						u32 bit_pos = 1<<(nb_bits-1);
						if (val == bit_pos)
							break;
					}
				}

				//then byte alignment of vui_payload
				gf_bs_read_int(orig, 1); //vui_payload_bit_equal_to_one
				gf_bs_align(orig);
			}
		}
		mod_vui_start_pos = gf_bs_get_bit_offset(mod) - mod_vui_start_pos;
		//check if we need explicit align
		if (!gf_bs_is_align(mod) || (mod_vui_start_pos != final_vvc_payload_size)) {
			gf_bs_write_int(mod, 1, 1); //vui_payload_bit_equal_to_one
			gf_bs_align(mod);
		}
		return;
	}

	/*no VUI in input bitstream but we just inserted one, set all remaining vui flags to 0*/
	if (!vui_present_flag) {
		gf_bs_write_int(mod, 0, 1);		/*chroma_location_info_present_flag */
		gf_bs_write_int(mod, 0, 1);		/*timing_info_present_flag*/
		gf_bs_write_int(mod, 0, 1);		/*nal_hrd_parameters_present*/
		gf_bs_write_int(mod, 0, 1);		/*vcl_hrd_parameters_present*/
		gf_bs_write_int(mod, 0, 1);		/*pic_struct_present*/
		gf_bs_write_int(mod, 0, 1);		/*bitstream_restriction*/
	}
	/*otherwise we copy over the bits from the input bitrate*/
}