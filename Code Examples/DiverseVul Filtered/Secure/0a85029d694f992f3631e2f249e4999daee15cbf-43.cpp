GF_Err gf_isom_nalu_sample_rewrite(GF_MediaBox *mdia, GF_ISOSample *sample, u32 sampleNumber, GF_MPEGVisualSampleEntryBox *entry)
{
	Bool is_hevc = GF_FALSE;
	//if only one sync given in the sample sync table, insert sps/pps/vps before cra/bla in hevc
//	Bool check_cra_bla = (mdia->information->sampleTable->SyncSample && mdia->information->sampleTable->SyncSample->nb_entries>1) ? 0 : 1;
	Bool check_cra_bla = GF_TRUE;
	Bool insert_nalu_delim = GF_TRUE;
	Bool force_sei_inspect = GF_FALSE;
	GF_Err e = GF_OK;
	GF_BitStream *sei_suffix_bs = NULL;
	Bool ps_transfered = GF_FALSE;
	u32 nal_size, nal_unit_size_field, extractor_mode;
	Bool rewrite_ps, rewrite_start_codes, insert_vdrd_code;
	u8 nal_type;
	u32 nal_hdr, sabt_ref, i, track_num;
	u32 temporal_id = 0;
	GF_ISOFile *file = mdia->mediaTrack->moov->mov;
	GF_TrackReferenceTypeBox *scal = NULL;

	Track_FindRef(mdia->mediaTrack, GF_ISOM_REF_SCAL, &scal);

	rewrite_ps = (mdia->mediaTrack->extractor_mode & GF_ISOM_NALU_EXTRACT_INBAND_PS_FLAG) ? GF_TRUE : GF_FALSE;
	rewrite_start_codes = (mdia->mediaTrack->extractor_mode & GF_ISOM_NALU_EXTRACT_ANNEXB_FLAG) ? GF_TRUE : GF_FALSE;
	insert_vdrd_code = (mdia->mediaTrack->extractor_mode & GF_ISOM_NALU_EXTRACT_VDRD_FLAG) ? GF_TRUE : GF_FALSE;
	if (!entry->svc_config && !entry->mvc_config && !entry->lhvc_config) insert_vdrd_code = GF_FALSE;
	extractor_mode = mdia->mediaTrack->extractor_mode&0x0000FFFF;

	if (mdia->mediaTrack->extractor_mode & GF_ISOM_NALU_EXTRACT_TILE_ONLY) {
		insert_nalu_delim = GF_FALSE;
	}

	track_num = 1 + gf_list_find(mdia->mediaTrack->moov->trackList, mdia->mediaTrack);

	if ( (extractor_mode != GF_ISOM_NALU_EXTRACT_INSPECT) && !(mdia->mediaTrack->extractor_mode & GF_ISOM_NALU_EXTRACT_TILE_ONLY) ) {
		u32 ref_track, di;
		//aggregate all sabt samples with the same DTS
		if (entry->lhvc_config && !entry->hevc_config && !(mdia->mediaTrack->extractor_mode & GF_ISOM_NALU_EXTRACT_LAYER_ONLY)) {
			if (gf_isom_get_reference_count(mdia->mediaTrack->moov->mov, track_num, GF_ISOM_REF_SCAL) <= 0) {
				//FIXME - for now we only support two layers (base + enh) in implicit
				if ( gf_isom_get_reference_count(mdia->mediaTrack->moov->mov, track_num, GF_ISOM_REF_BASE) >= 1) {
					GF_ISOSample *base_samp;
					gf_isom_get_reference(mdia->mediaTrack->moov->mov, track_num, GF_ISOM_REF_BASE, 1, &ref_track);
					switch (gf_isom_get_media_subtype(mdia->mediaTrack->moov->mov , ref_track, 1)) {
					case GF_ISOM_SUBTYPE_HVC1:
					case GF_ISOM_SUBTYPE_HVC2:
					case GF_ISOM_SUBTYPE_HEV1:
					case GF_ISOM_SUBTYPE_HEV2:

						if (!mdia->extracted_samp) {
							mdia->extracted_samp = gf_isom_sample_new();
							if (!mdia->extracted_samp) return GF_OUT_OF_MEM;
						}

						base_samp = gf_isom_get_sample_ex(mdia->mediaTrack->moov->mov, ref_track, sampleNumber + mdia->mediaTrack->sample_count_at_seg_start, &di, mdia->extracted_samp, NULL);
						if (base_samp && base_samp->data) {
							if (!sample->alloc_size || (sample->alloc_size<sample->dataLength+base_samp->dataLength) ) {
								sample->data = gf_realloc(sample->data, sample->dataLength+base_samp->dataLength);
								if (sample->alloc_size) sample->alloc_size = sample->dataLength+base_samp->dataLength;
							}
							memmove(sample->data + base_samp->dataLength, sample->data , sample->dataLength);
							memcpy(sample->data, base_samp->data, base_samp->dataLength);
							sample->dataLength += base_samp->dataLength;
						}
						Track_FindRef(mdia->mediaTrack, GF_ISOM_REF_BASE, &scal);
						break;
					}
				}
			}
		}

		sabt_ref = gf_isom_get_reference_count(mdia->mediaTrack->moov->mov, track_num, GF_ISOM_REF_SABT);
		if ((s32) sabt_ref > 0) {
			force_sei_inspect = GF_TRUE;
			for (i=0; i<sabt_ref; i++) {
				GF_ISOSample *tile_samp;
				gf_isom_get_reference(mdia->mediaTrack->moov->mov, track_num, GF_ISOM_REF_SABT, i+1, &ref_track);

				if (!mdia->extracted_samp) {
					mdia->extracted_samp = gf_isom_sample_new();
					if (!mdia->extracted_samp) return GF_OUT_OF_MEM;
				}

				tile_samp = gf_isom_get_sample_ex(mdia->mediaTrack->moov->mov, ref_track, sampleNumber + mdia->mediaTrack->sample_count_at_seg_start, &di, mdia->extracted_samp, NULL);
				if (tile_samp  && tile_samp ->data) {
					if (!sample->alloc_size || (sample->alloc_size<sample->dataLength+tile_samp->dataLength) ) {
						sample->data = gf_realloc(sample->data, sample->dataLength+tile_samp->dataLength);
						if (sample->alloc_size) sample->alloc_size = sample->dataLength+tile_samp->dataLength;
					}
					memcpy(sample->data + sample->dataLength, tile_samp->data, tile_samp->dataLength);
					sample->dataLength += tile_samp->dataLength;
				}
			}
		}
	}

	if ( gf_isom_get_reference_count(mdia->mediaTrack->moov->mov, track_num, GF_ISOM_REF_TBAS) >= 1) {
		u32 ref_track;
		u32 idx = gf_list_find(mdia->information->sampleTable->SampleDescription->child_boxes, entry);
		GF_TrackBox *tbas;
		gf_isom_get_reference(mdia->mediaTrack->moov->mov, track_num, GF_ISOM_REF_TBAS, 1, &ref_track);
		tbas = (GF_TrackBox *)gf_list_get(mdia->mediaTrack->moov->trackList, ref_track-1);
		entry = gf_list_get(tbas->Media->information->sampleTable->SampleDescription->child_boxes, idx);
	}


	if (sample->IsRAP < SAP_TYPE_2) {
		if (mdia->information->sampleTable->no_sync_found || (!sample->IsRAP && check_cra_bla) ) {
			sample->IsRAP = is_sample_idr(mdia, sample, entry);
		}
	}
	if (!sample->IsRAP)
		rewrite_ps = GF_FALSE;

	if (extractor_mode != GF_ISOM_NALU_EXTRACT_LAYER_ONLY)
		insert_vdrd_code = GF_FALSE;

	if (!entry) return GF_BAD_PARAM;

	//this is a compatible HEVC, don't insert VDRD, insert NALU delim
	if (entry->lhvc_config && entry->hevc_config)
		insert_vdrd_code = GF_FALSE;

	if (extractor_mode == GF_ISOM_NALU_EXTRACT_INSPECT) {
		if (!rewrite_ps && !rewrite_start_codes)
			return GF_OK;
	}

	nal_unit_size_field = 0;
	/*if svc rewrite*/
	if (entry->svc_config && entry->svc_config->config)
		nal_unit_size_field = entry->svc_config->config->nal_unit_size;
	/*if mvc rewrite*/
	if (entry->mvc_config && entry->mvc_config->config)
		nal_unit_size_field = entry->mvc_config->config->nal_unit_size;

	/*if lhvc rewrite*/
	else if (entry->lhvc_config && entry->lhvc_config->config)  {
		is_hevc = GF_TRUE;
		nal_unit_size_field = entry->lhvc_config->config->nal_unit_size;
	}

	/*otherwise do nothing*/
	else if (!rewrite_ps && !rewrite_start_codes && !scal && !force_sei_inspect) {
		return GF_OK;
	}

	if (!nal_unit_size_field) {
		if (entry->avc_config && entry->avc_config->config)
			nal_unit_size_field = entry->avc_config->config->nal_unit_size;
		else if (entry->lhvc_config && entry->lhvc_config->config) {
			nal_unit_size_field = entry->lhvc_config->config->nal_unit_size;
			is_hevc = GF_TRUE;
		}
		else if (entry->hevc_config && entry->hevc_config->config) {
			nal_unit_size_field = entry->hevc_config->config->nal_unit_size;
			is_hevc = GF_TRUE;
		}
	}

	if (!nal_unit_size_field) return GF_ISOM_INVALID_FILE;

	//setup PS rewriter
	if (!mdia->nalu_ps_bs)
		mdia->nalu_ps_bs = gf_bs_new(NULL, 0, GF_BITSTREAM_WRITE);
	gf_bs_seek(mdia->nalu_ps_bs, 0);

	//setup sample reader
	if (mdia->in_sample_buffer_alloc<sample->dataLength) {
		mdia->in_sample_buffer_alloc = sample->dataLength;
		mdia->in_sample_buffer = gf_realloc(mdia->in_sample_buffer, sample->dataLength);
	}
	memcpy(mdia->in_sample_buffer, sample->data, sample->dataLength);

	if (!mdia->nalu_parser) {
		mdia->nalu_parser = gf_bs_new(mdia->in_sample_buffer, sample->dataLength, GF_BITSTREAM_READ);
		if (!mdia->nalu_parser && sample->data) return GF_ISOM_INVALID_FILE;
	} else {
		e = gf_bs_reassign_buffer(mdia->nalu_parser, mdia->in_sample_buffer, sample->dataLength);
		if (e) return e;
	}
	//setup ouput
	if (!mdia->nalu_out_bs) {
		u8 *output;
		u32 outSize;
		mdia->nalu_out_bs = gf_bs_new(NULL, 0, GF_BITSTREAM_WRITE);
		gf_bs_get_content(mdia->nalu_out_bs, &output, &outSize);
	}

	gf_bs_reassign_buffer(mdia->nalu_out_bs, sample->data, sample->alloc_size ? sample->alloc_size : sample->dataLength);

	/*rewrite start code with NALU delim*/
	if (rewrite_start_codes) {

		//we are SVC, don't write NALU delim, only insert VDRD NALU
		if (insert_vdrd_code) {
			if (is_hevc) {
				//spec is not clear here, we don't insert an NALU AU delimiter before the layer starts since it breaks openHEVC
//				insert_nalu_delim=0;
			} else {
				gf_bs_write_int(mdia->nalu_out_bs, 1, 32);
				gf_bs_write_int(mdia->nalu_out_bs, GF_AVC_NALU_VDRD , 8);
				insert_nalu_delim=0;
			}
		}

		//AVC/HEVC base, insert NALU delim
		if (insert_nalu_delim) {
			gf_bs_write_int(mdia->nalu_out_bs, 1, 32);
			if (is_hevc) {
#ifndef GPAC_DISABLE_HEVC
				gf_bs_write_int(mdia->nalu_out_bs, 0, 1);
				gf_bs_write_int(mdia->nalu_out_bs, GF_HEVC_NALU_ACCESS_UNIT, 6);
				gf_bs_write_int(mdia->nalu_out_bs, insert_vdrd_code ? 1 : 0, 6); //we should pick the layerID of the following nalus ...
				gf_bs_write_int(mdia->nalu_out_bs, 1, 3); //nuh_temporal_id_plus1 - cannot be 0, we use 1 by default, and overwrite it if needed at the end

				/*pic-type - by default we signal all slice types possible*/
				gf_bs_write_int(mdia->nalu_out_bs, 2, 3);
				gf_bs_write_int(mdia->nalu_out_bs, 0, 5);
#endif
			} else {
				gf_bs_write_int(mdia->nalu_out_bs, (sample->data[0] & 0x60) | GF_AVC_NALU_ACCESS_UNIT, 8);
				gf_bs_write_int(mdia->nalu_out_bs, 0xF0 , 8); /*7 "all supported NALUs" (=111) + rbsp trailing (10000)*/;
			}
		}
	}

	if (rewrite_ps) {
		Bool has_vps = GF_FALSE;
		//in inspect mode or single-layer mode just use the xPS from this layer
		if (extractor_mode == GF_ISOM_NALU_EXTRACT_DEFAULT) {
			if (scal) {
				for (i=0; i<scal->trackIDCount; i++) {
					GF_TrackBox *a_track = GetTrackbyID(mdia->mediaTrack->moov, scal->trackIDs[i]);
					GF_MPEGVisualSampleEntryBox *an_entry = NULL;
					if (a_track && a_track->Media && a_track->Media->information && a_track->Media->information->sampleTable && a_track->Media->information->sampleTable->SampleDescription)
						an_entry = (GF_MPEGVisualSampleEntryBox*)gf_list_get(a_track->Media->information->sampleTable->SampleDescription->child_boxes, 0);

					if (an_entry)
						nalu_merge_ps(mdia->nalu_ps_bs, rewrite_start_codes, nal_unit_size_field, an_entry, is_hevc, &has_vps);
				}
			}
		}
		nalu_merge_ps(mdia->nalu_ps_bs, rewrite_start_codes, nal_unit_size_field, entry, is_hevc, &has_vps);


		if (is_hevc) {
			/*little optimization if we are not asked to start codes: copy over the sample*/
			if (!rewrite_start_codes && !entry->lhvc_config && !scal) {
				if (! ps_transfered) {
					nal_type = (sample->data[nal_unit_size_field] & 0x7E) >> 1;
					//temp fix - if we detect xPS in the beginning of the sample do NOT copy the ps bitstream
					//this is not correct since we are not sure whether they are the same xPS or not, but it crashes openHEVC ...
					switch (nal_type) {
#ifndef GPAC_DISABLE_HEVC
					case GF_HEVC_NALU_VID_PARAM:
					case GF_HEVC_NALU_SEQ_PARAM:
					case GF_HEVC_NALU_PIC_PARAM:
						break;
#endif
					default:
						gf_bs_transfer(mdia->nalu_out_bs, mdia->nalu_ps_bs, GF_TRUE);
						break;
					}
				}
				gf_bs_write_data(mdia->nalu_out_bs, mdia->in_sample_buffer, sample->dataLength);
				gf_bs_get_content_no_truncate(mdia->nalu_out_bs, &sample->data, &sample->dataLength, &sample->alloc_size);

				return GF_OK;
			}
		}
	} else {
		ps_transfered = GF_TRUE;
	}

	/*little optimization if we are not asked to rewrite extractors or start codes: copy over the sample*/
	if (!scal && !rewrite_start_codes && !rewrite_ps && !force_sei_inspect) {
		if (! ps_transfered)
		{
			gf_bs_transfer(mdia->nalu_out_bs, mdia->nalu_ps_bs, GF_TRUE);
		}
		gf_bs_write_data(mdia->nalu_out_bs, mdia->in_sample_buffer, sample->dataLength);
		gf_bs_get_content_no_truncate(mdia->nalu_out_bs, &sample->data, &sample->dataLength, &sample->alloc_size);
		return GF_OK;
	}

	if (!mdia->tmp_nal_copy_buffer) {
		mdia->tmp_nal_copy_buffer = gf_malloc(sizeof(char) * 4096);
		mdia->tmp_nal_copy_buffer_alloc = 4096;
	}


	while (gf_bs_available(mdia->nalu_parser)) {
		nal_size = gf_bs_read_int(mdia->nalu_parser, 8*nal_unit_size_field);
		if (gf_bs_get_position(mdia->nalu_parser) + nal_size > sample->dataLength) {
			GF_LOG(GF_LOG_WARNING, GF_LOG_CODING, ("Sample %u (size %u) rewrite: corrupted NAL Unit (size %u)\n", sampleNumber, sample->dataLength, nal_size));
			goto exit;
		}
		if (nal_size > mdia->tmp_nal_copy_buffer_alloc) {
			mdia->tmp_nal_copy_buffer_alloc = nal_size;
			mdia->tmp_nal_copy_buffer = (char*) gf_realloc(mdia->tmp_nal_copy_buffer, sizeof(char)*nal_size);
		}
		if (is_hevc) {
			nal_hdr = gf_bs_read_u16(mdia->nalu_parser);
			nal_type = (nal_hdr&0x7E00) >> 9;
		} else {
			nal_hdr = gf_bs_read_u8(mdia->nalu_parser);
			nal_type = nal_hdr & 0x1F;
		}

		if (is_hevc) {
#ifndef GPAC_DISABLE_HEVC
			GF_BitStream *write_to_bs = mdia->nalu_out_bs;
#endif

			if (!ps_transfered) {
				gf_bs_transfer(mdia->nalu_out_bs, mdia->nalu_ps_bs, GF_TRUE);
				ps_transfered = GF_TRUE;
			}

#ifndef GPAC_DISABLE_HEVC
			/*we already wrote this stuff*/
			if (nal_type==GF_HEVC_NALU_ACCESS_UNIT) {
				gf_bs_skip_bytes(mdia->nalu_parser, nal_size-2);
				continue;
			}
			switch (nal_type) {
			//extractor
			case 49:
				e = process_extractor(file, mdia, sampleNumber, sample->DTS, nal_size, nal_hdr, nal_unit_size_field, GF_TRUE, rewrite_ps, rewrite_start_codes, extractor_mode);
				if (e) goto exit;
				break;

			case GF_HEVC_NALU_SLICE_TSA_N:
			case GF_HEVC_NALU_SLICE_STSA_N:
			case GF_HEVC_NALU_SLICE_TSA_R:
			case GF_HEVC_NALU_SLICE_STSA_R:
				if (temporal_id < (nal_hdr & 0x7))
					temporal_id = (nal_hdr & 0x7);
				/*rewrite nal*/
				gf_bs_read_data(mdia->nalu_parser, mdia->tmp_nal_copy_buffer, nal_size-2);
				if (rewrite_start_codes)
					gf_bs_write_u32(mdia->nalu_out_bs, 1);
				else
					gf_bs_write_int(mdia->nalu_out_bs, nal_size, 8*nal_unit_size_field);

				gf_bs_write_u16(mdia->nalu_out_bs, nal_hdr);
				gf_bs_write_data(mdia->nalu_out_bs, mdia->tmp_nal_copy_buffer, nal_size-2);
				break;

			case GF_HEVC_NALU_SLICE_BLA_W_LP:
			case GF_HEVC_NALU_SLICE_BLA_W_DLP:
			case GF_HEVC_NALU_SLICE_BLA_N_LP:
			case GF_HEVC_NALU_SLICE_IDR_W_DLP:
			case GF_HEVC_NALU_SLICE_IDR_N_LP:
			case GF_HEVC_NALU_SLICE_CRA:
				//insert xPS before CRA/BLA
				if (check_cra_bla && !sample->IsRAP) {
					sample->IsRAP = sap_type_from_nal_type(nal_type);
					if (sei_suffix_bs) gf_bs_del(sei_suffix_bs);
					return gf_isom_nalu_sample_rewrite(mdia, sample, sampleNumber, entry);
				}
			default:
				/*rewrite nal*/
				if (nal_size<2) {
					GF_LOG(GF_LOG_ERROR, GF_LOG_CONTAINER, ("[iso file] Invalid nal size %d in sample %d\n", nal_type, sampleNumber));
					e = GF_NON_COMPLIANT_BITSTREAM;
					goto exit;
				}

				gf_bs_read_data(mdia->nalu_parser, mdia->tmp_nal_copy_buffer, nal_size-2);

				if (nal_type==GF_HEVC_NALU_SEI_SUFFIX) {
					if (!sei_suffix_bs) sei_suffix_bs = gf_bs_new(NULL, 0, GF_BITSTREAM_WRITE);
					write_to_bs = sei_suffix_bs;
				}

				if (rewrite_start_codes)
					gf_bs_write_u32(write_to_bs, 1);
				else
					gf_bs_write_int(write_to_bs, nal_size, 8*nal_unit_size_field);

				gf_bs_write_u16(write_to_bs, nal_hdr);
				gf_bs_write_data(write_to_bs, mdia->tmp_nal_copy_buffer, nal_size-2);
			}
#endif

			//done with HEVC
			continue;
		}

		switch(nal_type) {
		case GF_AVC_NALU_ACCESS_UNIT:
			/*we already wrote this stuff*/
			gf_bs_skip_bytes(mdia->nalu_parser, nal_size-1);
			continue;
		//extractor
		case 31:
			e = process_extractor(file, mdia, sampleNumber, sample->DTS, nal_size, nal_hdr, nal_unit_size_field, GF_FALSE, rewrite_ps, rewrite_start_codes, extractor_mode);
			if (e) goto exit;
			break;
//			case GF_AVC_NALU_SEI:
		case GF_AVC_NALU_SEQ_PARAM:
		case GF_AVC_NALU_PIC_PARAM:
		case GF_AVC_NALU_SEQ_PARAM_EXT:
		case GF_AVC_NALU_SVC_SUBSEQ_PARAM:
			// we will rewrite the sps/pps if and only if there is no sps/pps in bistream
			if (!ps_transfered) {
				ps_transfered = GF_TRUE;
			}
		default:
			if (!ps_transfered) {
				gf_bs_transfer(mdia->nalu_out_bs, mdia->nalu_ps_bs, GF_TRUE);
				ps_transfered = GF_TRUE;
			}
			gf_bs_read_data(mdia->nalu_parser, mdia->tmp_nal_copy_buffer, nal_size-1);
			if (rewrite_start_codes)
				gf_bs_write_u32(mdia->nalu_out_bs, 1);
			else
				gf_bs_write_int(mdia->nalu_out_bs, nal_size, 8*nal_unit_size_field);

			gf_bs_write_u8(mdia->nalu_out_bs, nal_hdr);
			gf_bs_write_data(mdia->nalu_out_bs, mdia->tmp_nal_copy_buffer, nal_size-1);
		}
	}

	if (sei_suffix_bs) {
		gf_bs_transfer(mdia->nalu_out_bs, sei_suffix_bs, GF_FALSE);
	}
	/*done*/
	gf_bs_get_content_no_truncate(mdia->nalu_out_bs, &sample->data, &sample->dataLength, &sample->alloc_size);

	/*rewrite temporal ID of AU Ddelim NALU (first one)*/
	if (rewrite_start_codes && is_hevc && temporal_id) {
		sample->data[6] = (sample->data[6] & 0xF8) | (temporal_id+1);
	}


exit:
	if (sei_suffix_bs)
		gf_bs_del(sei_suffix_bs);

	return e;
}