GF_Err naludmx_process(GF_Filter *filter)
{
	GF_NALUDmxCtx *ctx = gf_filter_get_udta(filter);
	GF_FilterPacket *pck;
	u8 *start;
	u32 nalu_before = ctx->nb_nalus;
	u32 nalu_store_before = 0;
	s32 remain;
	Bool is_eos = GF_FALSE;
	Bool drop_packet = GF_FALSE;
	u64 byte_offset = GF_FILTER_NO_BO;

	//always reparse duration
	if (!ctx->file_loaded)
		naludmx_check_dur(filter, ctx);

	pck = gf_filter_pid_get_packet(ctx->ipid);
	if (!ctx->resume_from && !pck) {
		if (gf_filter_pid_is_eos(ctx->ipid)) {
			if (ctx->nal_store_size) {
				if (!ctx->is_playing)
					return GF_OK;

				start = ctx->nal_store;
				remain = ctx->nal_store_size;
				is_eos = GF_TRUE;
				goto naldmx_flush;
			}
			if (ctx->first_pck_in_au) {
				naludmx_finalize_au_flags(ctx);
			}
			//single-frame stream
			if (!ctx->poc_diff) ctx->poc_diff = 1;
			ctx->strict_poc = STRICT_POC_OFF;
			naludmx_enqueue_or_dispatch(ctx, NULL, GF_TRUE);
			if (ctx->src_pck) gf_filter_pck_unref(ctx->src_pck);
			ctx->src_pck = NULL;
			if (!ctx->opid) return GF_EOS;

			gf_filter_pid_set_info(ctx->opid, GF_PROP_PID_MAX_NALU_SIZE, &PROP_UINT(ctx->max_nalu_size) );
			if (ctx->codecid==GF_CODECID_HEVC) {
				naludmx_set_hevc_oinf(ctx, ctx->max_temporal_id);
				naludmx_set_hevc_linf(ctx);
				gf_filter_pid_set_info_str(ctx->opid, "hevc:min_lid", &PROP_UINT(ctx->min_layer_id) );
			}
			if (ctx->opid)
				gf_filter_pid_set_eos(ctx->opid);
			return GF_EOS;
		}
		return GF_OK;
	}

	if (!ctx->is_playing && ctx->opid)
		return GF_OK;

	//if we have bytes from previous packet in the header, we cannot switch timing until we know what these bytes are
	if (!ctx->nal_store_size)
		naldmx_switch_timestamps(ctx, pck);

	nalu_store_before = ctx->nal_store_size;
	if (!ctx->resume_from && pck) {
		u32 pck_size;
		const u8 *data = gf_filter_pck_get_data(pck, &pck_size);
		if (ctx->nal_store_alloc < ctx->nal_store_size + pck_size) {
			ctx->nal_store_alloc = ctx->nal_store_size + pck_size;
			ctx->nal_store = gf_realloc(ctx->nal_store, sizeof(char)*ctx->nal_store_alloc);
			if (!ctx->nal_store) {
				ctx->nal_store_alloc = 0;
				return GF_OUT_OF_MEM;
			}
		}
		byte_offset = gf_filter_pck_get_byte_offset(pck);
		if (byte_offset != GF_FILTER_NO_BO)
			byte_offset -= ctx->nal_store_size;
		memcpy(ctx->nal_store + ctx->nal_store_size, data, sizeof(char)*pck_size);
		ctx->nal_store_size += pck_size;
		drop_packet = GF_TRUE;
	}
	start = ctx->nal_store;
	remain = ctx->nal_store_size;

	if (ctx->resume_from) {
		if (ctx->opid && gf_filter_pid_would_block(ctx->opid))
			return GF_OK;

		assert(ctx->resume_from < ctx->nal_store_size);
		start += ctx->resume_from;
		remain -= ctx->resume_from;
		ctx->resume_from = 0;

		if (!pck && gf_filter_pid_is_eos(ctx->ipid))
			is_eos = GF_TRUE;
	}

naldmx_flush:
	if (!ctx->bs_r) {
		ctx->bs_r = gf_bs_new(start, remain, GF_BITSTREAM_READ);

#ifndef GPAC_DISABLE_LOG
		if (ctx->bsdbg && gf_log_tool_level_on(GF_LOG_MEDIA, GF_LOG_DEBUG))
			gf_bs_set_logger(ctx->bs_r, naldmx_bs_log, ctx);
#endif

	} else {
		gf_bs_reassign_buffer(ctx->bs_r, start, remain);
	}

    assert(remain>=0);

	while (remain) {
		u8 *pck_data;
		u8 *nal_data;
		u32 nal_size;
		s32 current;
		Bool skip_nal = GF_FALSE;
		u32 sc_size=0;
		u32 nal_type = 0;
		u32 nal_ref_idc = 0;
		s32 next=0;
		u32 next_sc_size=0;
		s32 nal_parse_result;
		Bool slice_is_idr, slice_force_ref;
		Bool is_slice = GF_FALSE;
		Bool is_islice = GF_FALSE;
		Bool bottom_field_flag = GF_FALSE;
		Bool au_start;
		u32 avc_svc_subs_reserved = 0;
		u8 avc_svc_subs_priority = 0;
		Bool recovery_point_valid = GF_FALSE;
		u32 recovery_point_frame_cnt = 0;
		Bool bIntraSlice = GF_FALSE;
		GF_FilterSAPType au_sap_type = GF_FILTER_SAP_NONE;
		Bool slice_is_b = GF_FALSE;
		Bool check_dep = GF_FALSE;
		Bool force_au_flush = GF_FALSE;
		s32 slice_poc = 0;

		//not enough bytes to parse start code + nal hdr
		if (!is_eos && (remain<6)) {
			break;
		}

		//locate next start code
		current = gf_media_nalu_next_start_code(start, remain, &sc_size);
		if (current == remain)
			current = -1;

		//no start code, gontinue gathering data
		if (current<0) {
			break;
		}

		assert(current>=0);

		//unknown data before start of nal, may happen when tuning in, discard
		if (current) {
			assert(remain>=current);
			start += current;
			remain -= current;
			current = 0;
			naldmx_check_timestamp_switch(ctx, &nalu_store_before, current, &drop_packet, pck);
		}

		if (!remain)
			break;

		//not enough bytes to parse start code + nal hdr
		if (!is_eos && (remain<6)) {
			break;
		}

		nal_data = start + sc_size;
		nal_size = remain - sc_size;

		//figure out which nal we need to completely load
		if (ctx->codecid==GF_CODECID_HEVC) {
			nal_type = nal_data[0];
			nal_type = (nal_type & 0x7E) >> 1;

			switch (nal_type) {
			case GF_HEVC_NALU_VID_PARAM:
			case GF_HEVC_NALU_SEQ_PARAM:
			case GF_HEVC_NALU_PIC_PARAM:
				force_au_flush = GF_TRUE;
			case GF_HEVC_NALU_SEI_PREFIX:
			case GF_HEVC_NALU_SEI_SUFFIX:
				break;
			case GF_HEVC_NALU_SLICE_TRAIL_N:
			case GF_HEVC_NALU_SLICE_TSA_N:
			case GF_HEVC_NALU_SLICE_STSA_N:
			case GF_HEVC_NALU_SLICE_RADL_N:
			case GF_HEVC_NALU_SLICE_RASL_N:
			case GF_HEVC_NALU_SLICE_RSV_VCL_N10:
			case GF_HEVC_NALU_SLICE_RSV_VCL_N12:
			case GF_HEVC_NALU_SLICE_RSV_VCL_N14:
				check_dep = GF_TRUE;
				break;
			default:
				if (nal_type<GF_HEVC_NALU_VID_PARAM)
					nal_ref_idc = GF_TRUE;
				break;
			}
			//check if VPS/SPS/PPS lid/tid are greater than last seen VCL. If not, force a picture flush
			//not doing so could lead in dispatching the config changed before the current AU is sent
			if (force_au_flush) {
				if (!ctx->first_pck_in_au) {
					force_au_flush = GF_FALSE;
				} else {
					u8 layer_id = nal_data[0] & 1;
					layer_id<<=5;
					layer_id |= (nal_data[1] & 0xF8) >> 3;
					u8 temporal_id = nal_data[2] & 0x7;
					if (ctx->last_layer_id < layer_id)
						force_au_flush = GF_FALSE;
					else if (ctx->last_layer_id == layer_id) {
						if (ctx->last_temporal_id < temporal_id)
							force_au_flush = GF_FALSE;
					}
				}
			}
		} else if (ctx->codecid==GF_CODECID_VVC) {
			nal_type = nal_data[1]>>3;
			switch (nal_type) {
			case GF_VVC_NALU_OPI:
			case GF_VVC_NALU_DEC_PARAM:
			case GF_VVC_NALU_VID_PARAM:
			case GF_VVC_NALU_SEQ_PARAM:
			case GF_VVC_NALU_PIC_PARAM:
				force_au_flush = GF_TRUE;
			case GF_VVC_NALU_SEI_PREFIX:
			case GF_VVC_NALU_SEI_SUFFIX:
			case GF_VVC_NALU_APS_PREFIX:
			case GF_VVC_NALU_APS_SUFFIX:
			case GF_VVC_NALU_PIC_HEADER:
				break;

			case GF_VVC_NALU_SLICE_TRAIL:
			case GF_VVC_NALU_SLICE_STSA:
			case GF_VVC_NALU_SLICE_RADL:
			case GF_VVC_NALU_SLICE_RASL:
			case GF_VVC_NALU_SLICE_IDR_W_RADL:
			case GF_VVC_NALU_SLICE_IDR_N_LP:
			case GF_VVC_NALU_SLICE_CRA:
			case GF_VVC_NALU_SLICE_GDR:
				if (ctx->deps) {
					check_dep = GF_TRUE;
				}
				break;
			default:
				if (nal_type<GF_HEVC_NALU_VID_PARAM)
					nal_ref_idc = GF_TRUE;
				break;
			}

			//check if VPS/SPS/PPS/OPI/DEC lid/tid are greater than last seen VCL. If not, force a picture flush
			//not doing so could lead in dispatching the config changed before the current AU is sent
			if (force_au_flush) {
				if (!ctx->first_pck_in_au) {
					force_au_flush = GF_FALSE;
				} else {
					u8 layer_id = nal_data[0] & 0x3f;
					u8 temporal_id = (nal_data[1] & 0x7);
					if (ctx->last_layer_id < layer_id)
						force_au_flush = GF_FALSE;
					else if (ctx->last_layer_id == layer_id) {
						if (ctx->last_temporal_id < temporal_id)
							force_au_flush = GF_FALSE;
					}
				}
			}
		} else {
			nal_type = nal_data[0] & 0x1F;
			nal_ref_idc = (nal_data[0] & 0x60) >> 5;
		}

		//locate next NAL start
		next = gf_media_nalu_next_start_code(nal_data, nal_size, &next_sc_size);
		if (!is_eos && (next == nal_size) && !ctx->full_au_source) {
			next = -1;
		}

		//next nal start not found, wait
		if (next<0) {
			break;
		}

		//this is our exact NAL size, without start code
		nal_size = next;

		if (ctx->codecid==GF_CODECID_HEVC) {
			nal_parse_result = naludmx_parse_nal_hevc(ctx, nal_data, nal_size, &skip_nal, &is_slice, &is_islice);
		} else if (ctx->codecid==GF_CODECID_VVC) {
			nal_parse_result = naludmx_parse_nal_vvc(ctx, nal_data, nal_size, &skip_nal, &is_slice, &is_islice);
		} else {
			nal_parse_result = naludmx_parse_nal_avc(ctx, nal_data, nal_size, nal_type, &skip_nal, &is_slice, &is_islice);
		}

		//dispatch right away if analyze
		if (ctx->analyze) {
			skip_nal = GF_FALSE;
			ctx->sei_buffer_size = 0;
		}

		//new frame - if no slices, we detected the new frame on AU delimiter, don't flush new frame !
		if ((nal_parse_result>0) && !ctx->first_slice_in_au) {
			naludmx_end_access_unit(ctx);
		}

		naludmx_check_pid(filter, ctx, force_au_flush);
		if (!ctx->opid) skip_nal = GF_TRUE;

		if (skip_nal) {
			nal_size += sc_size;
			assert((u32) remain >= nal_size);
			start += nal_size;
			remain -= nal_size;
			naldmx_check_timestamp_switch(ctx, &nalu_store_before, nal_size, &drop_packet, pck);
			continue;
		}

		if (!ctx->is_playing) {
			ctx->resume_from = (u32) (start - ctx->nal_store);
            assert(ctx->resume_from<=ctx->nal_store_size);
			GF_LOG(GF_LOG_DEBUG, GF_LOG_MEDIA, ("[%s] not yet playing\n", ctx->log_name));

			if (drop_packet)
				gf_filter_pid_drop_packet(ctx->ipid);
			return GF_OK;
		}
		if (ctx->in_seek) {
			u64 nb_frames_at_seek = (u64) (ctx->start_range * ctx->cur_fps.num);
			if (ctx->cts + ctx->cur_fps.den >= nb_frames_at_seek) {
				//u32 samples_to_discard = (ctx->cts + ctx->dts_inc) - nb_samples_at_seek;
				if (ctx->seek_gdr_count)
					ctx->seek_gdr_count--;
				else
					ctx->in_seek = GF_FALSE;
			}
		}

		if (nal_parse_result<0) {
			if (byte_offset != GF_FILTER_NO_BO) {
				u64 bo = byte_offset;
				bo += (start - ctx->nal_store);

				GF_LOG(GF_LOG_ERROR, GF_LOG_MEDIA, ("[%s] Error parsing NAL Unit %d (byte offset "LLU" size %d type %d frame %d last POC %d) - skipping\n", ctx->log_name, ctx->nb_nalus, bo, nal_size, nal_type, ctx->nb_frames, ctx->last_poc));
			} else {
				GF_LOG(GF_LOG_ERROR, GF_LOG_MEDIA, ("[%s] Error parsing NAL Unit %d (size %d type %d frame %d last POC %d) - skipping\n", ctx->log_name, ctx->nb_nalus, nal_size, nal_type, ctx->nb_frames, ctx->last_poc));
			}
			nal_size += sc_size;
			assert((u32) remain >= nal_size);
			start += nal_size;
			remain -= nal_size;
			naldmx_check_timestamp_switch(ctx, &nalu_store_before, nal_size, &drop_packet, pck);
			continue;
		}

		if (check_dep) {
			if ((ctx->codecid==GF_CODECID_HEVC) && ctx->hevc_state->s_info.sps) {
				HEVC_VPS *vps;
				u32 temporal_id = nal_data[1] & 0x7;
				vps = & ctx->hevc_state->vps[ctx->hevc_state->s_info.sps->vps_id];
				if (temporal_id + 1 < vps->max_sub_layers) {
					nal_ref_idc = GF_TRUE;
				}
			} else if (ctx->codecid==GF_CODECID_VVC) {
				if (ctx->vvc_state->s_info.non_ref_pic) {
					nal_ref_idc = GF_FALSE;
				} else {
					//todo
					nal_ref_idc = GF_TRUE;
				}
			}
		}


		if (is_islice) ctx->has_islice = GF_TRUE;

		//store all variables needed to compute POC/CTS and sample SAP and recovery info
		if (ctx->codecid==GF_CODECID_HEVC) {
#ifndef GPAC_DISABLE_HEVC
			slice_is_idr = gf_hevc_slice_is_IDR(ctx->hevc_state);

			recovery_point_valid = ctx->hevc_state->sei.recovery_point.valid;
			recovery_point_frame_cnt = ctx->hevc_state->sei.recovery_point.frame_cnt;
			bIntraSlice = gf_hevc_slice_is_intra(ctx->hevc_state);

			au_sap_type = GF_FILTER_SAP_NONE;
			if (gf_hevc_slice_is_IDR(ctx->hevc_state)) {
				au_sap_type = GF_FILTER_SAP_1;
			}
			else {
				switch (ctx->hevc_state->s_info.nal_unit_type) {
				case GF_HEVC_NALU_SLICE_BLA_W_LP:
				case GF_HEVC_NALU_SLICE_BLA_W_DLP:
					au_sap_type = GF_FILTER_SAP_3;
					break;
				case GF_HEVC_NALU_SLICE_BLA_N_LP:
					au_sap_type = GF_FILTER_SAP_1;
					break;
				case GF_HEVC_NALU_SLICE_CRA:
					au_sap_type = GF_FILTER_SAP_3;
					break;
				}
			}

			slice_poc = ctx->hevc_state->s_info.poc;

			/*need to store TS offsets*/
			switch (ctx->hevc_state->s_info.slice_type) {
			case GF_AVC_TYPE_B:
			case GF_AVC_TYPE2_B:
				slice_is_b = GF_TRUE;
				break;
			}
#endif // GPAC_DISABLE_HEVC
		} else if (ctx->codecid==GF_CODECID_VVC) {
			slice_is_idr = gf_vvc_slice_is_ref(ctx->vvc_state);
			recovery_point_valid = ctx->vvc_state->s_info.recovery_point_valid;
			recovery_point_frame_cnt = ctx->vvc_state->s_info.gdr_recovery_count;

//			commented, set below
//			if (ctx->vvc_state->s_info.irap_or_gdr_pic && !ctx->vvc_state->s_info.gdr_pic)
//				bIntraSlice = GF_TRUE; //gf_hevc_slice_is_intra(ctx->hevc_state);

			au_sap_type = GF_FILTER_SAP_NONE;
			if (ctx->vvc_state->s_info.irap_or_gdr_pic && !ctx->vvc_state->s_info.gdr_pic) {
				bIntraSlice = GF_TRUE;

				switch (ctx->vvc_state->s_info.nal_unit_type) {
				case GF_VVC_NALU_SLICE_CRA:
					au_sap_type = GF_FILTER_SAP_3;
					slice_is_idr = GF_FALSE;
					break;
				case GF_VVC_NALU_SLICE_IDR_N_LP:
					au_sap_type = GF_FILTER_SAP_1;
					break;
				case GF_VVC_NALU_SLICE_IDR_W_RADL:
					au_sap_type = GF_FILTER_SAP_2;
					slice_is_idr = GF_TRUE;
					break;
				}
			} else {
				switch (ctx->vvc_state->s_info.nal_unit_type) {
				case GF_VVC_NALU_SLICE_IDR_N_LP:
					au_sap_type = GF_FILTER_SAP_1;
					slice_is_idr = GF_TRUE;
					bIntraSlice = GF_TRUE;
					break;
				case GF_VVC_NALU_SLICE_CRA:
					au_sap_type = GF_FILTER_SAP_3;
					bIntraSlice = GF_TRUE;
					break;
				case GF_VVC_NALU_SLICE_IDR_W_RADL:
					bIntraSlice = GF_TRUE;
					if (ctx->vvc_state->s_info.gdr_pic) {
						au_sap_type = GF_FILTER_SAP_3;
					} else {
						au_sap_type = GF_FILTER_SAP_1;
						slice_is_idr = GF_TRUE;
					}
					break;
				}
			}

			slice_poc = ctx->vvc_state->s_info.poc;

			/*need to store TS offsets*/
			switch (ctx->vvc_state->s_info.slice_type) {
			case GF_AVC_TYPE_B:
			case GF_AVC_TYPE2_B:
				slice_is_b = GF_TRUE;
				break;
			}
		} else {

			/*fixme - we need finer grain for priority*/
			if ((nal_type==GF_AVC_NALU_SVC_PREFIX_NALU) || (nal_type==GF_AVC_NALU_SVC_SLICE)) {
				if (!ctx->is_mvc) {
					unsigned char *p = (unsigned char *) start;
					// RefPicFlag
					avc_svc_subs_reserved |= (p[0] & 0x60) ? 0x80000000 : 0;
					// RedPicFlag TODO: not supported, would require to parse NAL unit payload
					avc_svc_subs_reserved |= (0) ? 0x40000000 : 0;
					// VclNALUnitFlag
					avc_svc_subs_reserved |= (1<=nal_type && nal_type<=5) || (nal_type==GF_AVC_NALU_SVC_PREFIX_NALU) || (nal_type==GF_AVC_NALU_SVC_SLICE) ? 0x20000000 : 0;
					// use values of IdrFlag and PriorityId directly from SVC extension header
					avc_svc_subs_reserved |= p[1] << 16;
					// use values of DependencyId and QualityId directly from SVC extension header
					avc_svc_subs_reserved |= p[2] << 8;
					// use values of TemporalId and UseRefBasePicFlag directly from SVC extension header
					avc_svc_subs_reserved |= p[3] & 0xFC;
					// StoreBaseRepFlag TODO: SVC FF mentions a store_base_rep_flag which cannot be found in SVC spec
					avc_svc_subs_reserved |= (0) ? 0x00000002 : 0;

					// priority_id (6 bits) in SVC has inverse meaning -> lower value means higher priority - invert it and scale it to 8 bits
					avc_svc_subs_priority = (63 - (p[1] & 0x3F)) << 2;
				}
				if (nal_type==GF_AVC_NALU_SVC_PREFIX_NALU) {
                    if (ctx->svc_prefix_buffer_size) {
                        GF_LOG(GF_LOG_WARNING, GF_LOG_MEDIA, ("[%s] broken bitstream, two consecutive SVC prefix NALU without SVC slice in-between\n", ctx->log_name));
                        ctx->svc_prefix_buffer_size = 0;
                    }

					/* remember reserved and priority value */
					ctx->svc_nalu_prefix_reserved = avc_svc_subs_reserved;
					ctx->svc_nalu_prefix_priority = avc_svc_subs_priority;

					ctx->svc_prefix_buffer_size = nal_size;
					if (ctx->svc_prefix_buffer_size > ctx->svc_prefix_buffer_alloc) {
						ctx->svc_prefix_buffer_alloc = ctx->svc_prefix_buffer_size;
						ctx->svc_prefix_buffer = gf_realloc(ctx->svc_prefix_buffer, ctx->svc_prefix_buffer_size);
					}
					memcpy(ctx->svc_prefix_buffer, start+sc_size, ctx->svc_prefix_buffer_size);

					assert( (u32) remain >= sc_size + nal_size);
					start += sc_size + nal_size;
					remain -= sc_size + nal_size;
					continue;
				}
			} else if (is_slice) {
				// RefPicFlag
				avc_svc_subs_reserved |= (start[0] & 0x60) ? 0x80000000 : 0;
				// VclNALUnitFlag
				avc_svc_subs_reserved |= (1<=nal_type && nal_type<=5) || (nal_type==GF_AVC_NALU_SVC_PREFIX_NALU) || (nal_type==GF_AVC_NALU_SVC_SLICE) ? 0x20000000 : 0;
				avc_svc_subs_priority = 0;
			}

			if (is_slice && ctx->avc_state->s_info.field_pic_flag) {
				ctx->is_paff = GF_TRUE;
				bottom_field_flag = ctx->avc_state->s_info.bottom_field_flag;
			}

			slice_is_idr = (ctx->avc_state->s_info.nal_unit_type==GF_AVC_NALU_IDR_SLICE) ? GF_TRUE : GF_FALSE;

			recovery_point_valid = ctx->avc_state->sei.recovery_point.valid;
			recovery_point_frame_cnt = ctx->avc_state->sei.recovery_point.frame_cnt;
			bIntraSlice = gf_avc_slice_is_intra(ctx->avc_state);

			au_sap_type = GF_FILTER_SAP_NONE;
			if (ctx->avc_state->s_info.nal_unit_type == GF_AVC_NALU_IDR_SLICE)
				au_sap_type = GF_FILTER_SAP_1;

			slice_poc = ctx->avc_state->s_info.poc;
			/*need to store TS offsets*/
			switch (ctx->avc_state->s_info.slice_type) {
			case GF_AVC_TYPE_B:
			case GF_AVC_TYPE2_B:
				slice_is_b = GF_TRUE;
				break;
			}
		}

		if (is_slice) {
			Bool first_in_au = ctx->first_slice_in_au;

			if (slice_is_idr)
				ctx->nb_idr++;

			if (au_sap_type==GF_FILTER_SAP_3)
				ctx->nb_cra++;

			slice_force_ref = GF_FALSE;

			/*we only indicate TRUE IDRs for sync samples (cf AVC file format spec).
			SEI recovery should be used to build sampleToGroup & RollRecovery tables*/
			if (ctx->first_slice_in_au) {
				ctx->first_slice_in_au = GF_FALSE;
				if (recovery_point_valid) {
					ctx->sei_recovery_frame_count = recovery_point_frame_cnt;

					/*we allow to mark I-frames as sync on open-GOPs (with sei_recovery_frame_count=0) when forcing sync even when the SEI RP is not available*/
					if (!recovery_point_frame_cnt && bIntraSlice) {
						ctx->has_islice = 1;
						if (ctx->use_opengop_gdr == 1) {
							ctx->use_opengop_gdr = 2; /*avoid message flooding*/
							GF_LOG(GF_LOG_WARNING, GF_LOG_MEDIA, ("[%s] No valid SEI Recovery Point found although needed - forcing\n", ctx->log_name));
						}
					}
					if (ctx->codecid==GF_CODECID_HEVC) {
						ctx->hevc_state->sei.recovery_point.valid = 0;
					} else if (ctx->codecid==GF_CODECID_VVC) {
						ctx->vvc_state->s_info.recovery_point_valid = 0;
					} else {
						ctx->avc_state->sei.recovery_point.valid = 0;
					}
					if (bIntraSlice && ctx->force_sync && (ctx->sei_recovery_frame_count==0))
						slice_force_ref = GF_TRUE;
				}
				ctx->au_sap = au_sap_type;
				ctx->bottom_field_flag = bottom_field_flag;
			}

			if (slice_poc < ctx->poc_shift) {

				u32 i, count = gf_list_count(ctx->pck_queue);
				for (i=0; i<count; i++) {
					u64 dts, cts;
					GF_FilterPacket *q_pck = gf_list_get(ctx->pck_queue, i);
					assert(q_pck);
					dts = gf_filter_pck_get_dts(q_pck);
					if (dts == GF_FILTER_NO_TS) continue;
					cts = gf_filter_pck_get_cts(q_pck);
					cts += ctx->poc_shift;
					cts -= slice_poc;
					gf_filter_pck_set_cts(q_pck, cts);
				}

				ctx->poc_shift = slice_poc;
			}

			/*if #pics, compute smallest POC increase*/
			if (slice_poc != ctx->last_poc) {
				s32 pdiff = ABS(ctx->last_poc - slice_poc);

				if ((slice_poc < 0) && !ctx->last_poc)
					ctx->poc_diff = 0;
				else if ((slice_poc < 0) && (-slice_poc < ctx->poc_diff)) {
					pdiff = -slice_poc;
					ctx->poc_diff = 0;
				}

				if (!ctx->poc_diff || (ctx->poc_diff > (s32) pdiff ) ) {
					ctx->poc_diff = pdiff;
					ctx->poc_probe_done = GF_FALSE;
				} else if (first_in_au) {
					//second frame with the same poc diff, we should be able to properly recompute CTSs
					ctx->poc_probe_done = GF_TRUE;
				}
				ctx->last_poc = slice_poc;
			}
			GF_LOG(GF_LOG_DEBUG, GF_LOG_MEDIA, ("[%s] POC is %d - min poc diff %d - slice is IDR %d (SAP %d)\n", ctx->log_name, slice_poc, ctx->poc_diff, slice_is_idr, au_sap_type));

			/*ref slice, reset poc*/
			if (slice_is_idr) {
				if (first_in_au) {
					Bool temp_poc_diff = GF_FALSE;
					//two consecutive IDRs, force poc_diff to 1 if 0 (when we have intra-only) to force frame dispatch
					if (ctx->last_frame_is_idr && !ctx->poc_diff) {
						temp_poc_diff = GF_TRUE;
						ctx->poc_diff = 1;
					}
					//new ref frame, dispatch all pending packets
					naludmx_enqueue_or_dispatch(ctx, NULL, GF_TRUE);

					//if IDR with DLP (sap2), only reset poc probing if the poc is below current max poc
					//otherwise assume no diff in poc
					if ((au_sap_type == GF_FILTER_SAP_2) && (ctx->max_last_poc >= ctx->last_poc) ){
						ctx->au_sap2_poc_reset = GF_TRUE;
					}

					if ((au_sap_type == GF_FILTER_SAP_1) || ctx->au_sap2_poc_reset) {
						if (!ctx->au_sap2_poc_reset)
							ctx->last_poc = 0;

						ctx->max_last_poc = ctx->last_poc;
						ctx->max_last_b_poc = ctx->last_poc;
						ctx->poc_shift = 0;
						//force probing of POC diff, this will prevent dispatching frames with wrong CTS until we have a clue of min poc_diff used
						ctx->poc_probe_done = 0;
					}
					ctx->last_frame_is_idr = GF_TRUE;
					if (temp_poc_diff)
						ctx->poc_diff = 0;
				}
			}
			/*forced ref slice*/
			else if (slice_force_ref) {
				ctx->last_frame_is_idr = GF_FALSE;
				if (first_in_au) {
					//new ref frame, dispatch all pending packets
					naludmx_enqueue_or_dispatch(ctx, NULL, GF_TRUE);

					/*adjust POC shift as sample will now be marked as sync, so we must store poc as if IDR (eg POC=0) for our CTS offset computing to be correct*/
					ctx->poc_shift = slice_poc;

					//force probing of POC diff, this will prevent dispatching frames with wrong CTS until we have a clue of min poc_diff used
					ctx->poc_probe_done = 0;
				}
			}
			/*strictly less - this is a new P slice*/
			else if (ctx->max_last_poc < ctx->last_poc) {
				ctx->max_last_b_poc = 0;
				ctx->max_last_poc = ctx->last_poc;
				ctx->last_frame_is_idr = GF_FALSE;
			}
			/*stricly greater*/
			else if (slice_is_b && (ctx->max_last_poc > ctx->last_poc)) {
				ctx->last_frame_is_idr = GF_FALSE;
				if (!ctx->max_last_b_poc) {
					ctx->max_last_b_poc = ctx->last_poc;
				}
				/*if same poc than last max, this is a B-slice*/
				else if (ctx->last_poc > ctx->max_last_b_poc) {
					ctx->max_last_b_poc = ctx->last_poc;
				}
				/*otherwise we had a B-slice reference: do nothing*/
			} else {
				ctx->last_frame_is_idr = GF_FALSE;
			}


			if (ctx->deps) {
				if (nal_ref_idc) {
					ctx->has_ref_slices = GF_TRUE;
				}
				if ((ctx->codecid==GF_CODECID_AVC) && (ctx->avc_state->s_info.redundant_pic_cnt) ) {
					ctx->has_redundant = GF_TRUE;
				}
			}
		}


		au_start = ctx->first_pck_in_au ? GF_FALSE : GF_TRUE;

		if (ctx->has_initial_aud) {
			u32 audelim_size = (ctx->codecid!=GF_CODECID_AVC) ? 3 : 2;
			/*dst_pck = */naludmx_start_nalu(ctx, audelim_size, GF_FALSE, &au_start, &pck_data);
			memcpy(pck_data + ctx->nal_length , ctx->init_aud, audelim_size);
			ctx->has_initial_aud = GF_FALSE;
			if (ctx->subsamples) {
				naludmx_add_subsample(ctx, audelim_size, avc_svc_subs_priority, avc_svc_subs_reserved);
			}
		}
		if (ctx->sei_buffer_size) {
			//sei buffer is already nal size prefixed
			/*dst_pck = */naludmx_start_nalu(ctx, ctx->sei_buffer_size, GF_TRUE, &au_start, &pck_data);
			memcpy(pck_data, ctx->sei_buffer, ctx->sei_buffer_size);
			if (ctx->subsamples) {
				naludmx_add_subsample(ctx, ctx->sei_buffer_size - ctx->nal_length, avc_svc_subs_priority, avc_svc_subs_reserved);
			}
			ctx->sei_buffer_size = 0;
		}

		if (ctx->svc_prefix_buffer_size) {
			/*dst_pck = */naludmx_start_nalu(ctx, ctx->svc_prefix_buffer_size, GF_FALSE, &au_start, &pck_data);
			memcpy(pck_data + ctx->nal_length, ctx->svc_prefix_buffer, ctx->svc_prefix_buffer_size);
			if (ctx->subsamples) {
				naludmx_add_subsample(ctx, ctx->svc_prefix_buffer_size, ctx->svc_nalu_prefix_priority, ctx->svc_nalu_prefix_reserved);
			}
			ctx->svc_prefix_buffer_size = 0;
		}

		//nalu size field
		/*dst_pck = */naludmx_start_nalu(ctx, (u32) nal_size, GF_FALSE, &au_start, &pck_data);
		pck_data += ctx->nal_length;

		//add subsample info before touching the size
		if (ctx->subsamples) {
			naludmx_add_subsample(ctx, (u32) nal_size, avc_svc_subs_priority, avc_svc_subs_reserved);
		}


		//bytes only come from the data packet
		memcpy(pck_data, nal_data, (size_t) nal_size);

		nal_size += sc_size;
		start += nal_size;
		remain -= nal_size;
		naldmx_check_timestamp_switch(ctx, &nalu_store_before, nal_size, &drop_packet, pck);

		//don't demux too much of input, abort when we would block. This avoid dispatching
		//a huge number of frames in a single call
		if (remain && gf_filter_pid_would_block(ctx->opid)) {
			ctx->resume_from = (u32) (start - ctx->nal_store);
			assert(ctx->resume_from <= ctx->nal_store_size);
			assert(ctx->resume_from == ctx->nal_store_size - remain);
			if (drop_packet)
				gf_filter_pid_drop_packet(ctx->ipid);
			return GF_OK;
		}
	}

	if (remain) {
		if (is_eos && (remain == ctx->nal_store_size)) {
			GF_LOG(GF_LOG_WARNING, GF_LOG_MEDIA, ("[%s] Incomplete last NAL and eos, discarding\n", ctx->log_name));
			remain = 0;
		} else {
			assert((u32) remain<=ctx->nal_store_size);
			memmove(ctx->nal_store, start, remain);
		}
	}
	ctx->nal_store_size = remain;

	if (drop_packet)
		gf_filter_pid_drop_packet(ctx->ipid);

	if (is_eos)
		return naludmx_process(filter);

	if ((ctx->nb_nalus>nalu_before) && gf_filter_reporting_enabled(filter)) {
		char szStatus[1024];

		sprintf(szStatus, "%s %dx%d % 10d NALU % 8d I % 8d P % 8d B % 8d SEI", ctx->log_name, ctx->width, ctx->height, ctx->nb_nalus, ctx->nb_i, ctx->nb_p, ctx->nb_b, ctx->nb_sei);
		gf_filter_update_status(filter, -1, szStatus);
	}
	if (ctx->full_au_source && ctx->poc_probe_done) {
		if (ctx->first_pck_in_au)
			naludmx_finalize_au_flags(ctx);

		naludmx_enqueue_or_dispatch(ctx, NULL, GF_TRUE);
	}
	return GF_OK;
}