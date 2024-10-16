static GF_Err process_extractor(GF_ISOFile *file, GF_MediaBox *mdia, u32 sampleNumber, u64 sampleDTS, u32 nal_size, u16 nal_hdr, u32 nal_unit_size_field, Bool is_hevc, Bool rewrite_ps, Bool rewrite_start_codes, u32 extractor_mode)
{
	GF_Err e;
	u32 di, ref_track_index, ref_track_num, data_offset, data_length, cur_extract_mode, ref_extract_mode, ref_nalu_size, nb_bytes_nalh;
	GF_TrackReferenceTypeBox *dpnd;
	GF_TrackBox *ref_trak;
	s8 sample_offset;
	u32 last_byte, ref_sample_num, prev_ref_sample_num;
	Bool header_written = GF_FALSE;
	nb_bytes_nalh = is_hevc ? 2 : 1;

	switch (extractor_mode) {
	case 0:
		last_byte = (u32) gf_bs_get_position(mdia->nalu_parser) + nal_size - (is_hevc ? 2 : 1);
		if (!is_hevc) gf_bs_read_int(mdia->nalu_parser, 24); //1 byte for HEVC , 3 bytes for AVC of NALUHeader in extractor
		while (gf_bs_get_position(mdia->nalu_parser) < last_byte) {
			u32 xmode = 0;
			//hevc extractors use constructors
			if (is_hevc) xmode = gf_bs_read_u8(mdia->nalu_parser);
			if (xmode) {
				u8 done=0, len = gf_bs_read_u8(mdia->nalu_parser);
				while (done<len) {
					u8 c = gf_bs_read_u8(mdia->nalu_parser);
					done++;
					if (header_written) {
						gf_bs_write_u8(mdia->nalu_out_bs, c);
					} else if (done==nal_unit_size_field) {
						if (rewrite_start_codes) {
							gf_bs_write_int(mdia->nalu_out_bs, 1, 32);
						} else {
							gf_bs_write_u8(mdia->nalu_out_bs, c);
						}
						header_written = GF_TRUE;
					} else if (!rewrite_start_codes) {
						gf_bs_write_u8(mdia->nalu_out_bs, c);
					}
				}
				continue;
			}

			ref_track_index = gf_bs_read_u8(mdia->nalu_parser);
			sample_offset = (s8) gf_bs_read_int(mdia->nalu_parser, 8);
			data_offset = gf_bs_read_int(mdia->nalu_parser, nal_unit_size_field*8);
			data_length = gf_bs_read_int(mdia->nalu_parser, nal_unit_size_field*8);

			Track_FindRef(mdia->mediaTrack, GF_ISOM_REF_SCAL, &dpnd);
			ref_track_num = 0;
			if (dpnd && ref_track_index && (ref_track_index<=dpnd->trackIDCount))
				ref_track_num = gf_isom_get_track_by_id(file, dpnd->trackIDs[ref_track_index-1]);

			if (!ref_track_num) {
				GF_LOG(GF_LOG_INFO, GF_LOG_CONTAINER, ("ISOBMF: Extractor target track is not present in file - skipping.\n"));
				return GF_OK;
			}

			cur_extract_mode = gf_isom_get_nalu_extract_mode(file, ref_track_num);

			//we must be in inspect mode only otherwise the reference sample will not be the one stored on file (change in start codes, PS inserted or other NALUs inserted)
			//and this will corrupt extraction (wrong data offsets)
			ref_extract_mode = GF_ISOM_NALU_EXTRACT_INSPECT;
			gf_isom_set_nalu_extract_mode(file, ref_track_num, ref_extract_mode);

			ref_trak = gf_isom_get_track_from_file(file, ref_track_num);
			if (!ref_trak) return GF_ISOM_INVALID_FILE;

			if (!mdia->extracted_samp) {
				mdia->extracted_samp = gf_isom_sample_new();
				if (!mdia->extracted_samp) return GF_IO_ERR;
			}
			if (!mdia->extracted_bs) {
				mdia->extracted_bs = gf_bs_new("a", 1, GF_BITSTREAM_READ);
				if (!mdia->extracted_bs) return GF_IO_ERR;
			}

			e = stbl_findEntryForTime(ref_trak->Media->information->sampleTable, sampleDTS, 0, &ref_sample_num, &prev_ref_sample_num);
			if (e) return e;
			if (!ref_sample_num) ref_sample_num = prev_ref_sample_num;
			if (!ref_sample_num) return GF_ISOM_INVALID_FILE;
			if ((sample_offset<0) && (ref_sample_num > (u32) -sample_offset)) return GF_ISOM_INVALID_FILE;
			ref_sample_num = (u32) ( (s32) ref_sample_num + sample_offset);

			e = Media_GetSample(ref_trak->Media, ref_sample_num, &mdia->extracted_samp, &di, GF_FALSE, NULL);
			if (e) return e;
			if (!mdia->extracted_samp->alloc_size)
				mdia->extracted_samp->alloc_size = mdia->extracted_samp->dataLength;
#if 0
			if (!header_written && rewrite_start_codes) {
				gf_bs_write_int(dst_bs, 1, 32);
				if (is_hevc) {
					gf_bs_write_int(dst_bs, 0, 1);
					gf_bs_write_int(dst_bs, GF_HEVC_NALU_ACCESS_UNIT, 6);
					gf_bs_write_int(dst_bs, 0, 9);
					/*pic-type - by default we signal all slice types possible*/
					gf_bs_write_int(dst_bs, 2, 3);
					gf_bs_write_int(dst_bs, 0, 5);
				} else {
					gf_bs_write_int(dst_bs, (ref_samp->data[0] & 0x60) | GF_AVC_NALU_ACCESS_UNIT, 8);
					gf_bs_write_int(dst_bs, 0xF0 , 8); /*7 "all supported NALUs" (=111) + rbsp trailing (10000)*/;
				}
			}
#endif
			gf_bs_reassign_buffer(mdia->extracted_bs, mdia->extracted_samp->data + data_offset, mdia->extracted_samp->dataLength - data_offset);

			if (mdia->extracted_samp->dataLength - data_offset >= data_length) {

				while (data_length && gf_bs_available(mdia->extracted_bs)) {
					if (!header_written) {
						ref_nalu_size = gf_bs_read_int(mdia->extracted_bs, 8*nal_unit_size_field);

						assert(data_length>nal_unit_size_field);
						data_length -= nal_unit_size_field;
						if (data_length > gf_bs_available(mdia->extracted_bs)) {
							data_length = (u32)gf_bs_available(mdia->extracted_bs);
						}
					} else {
						ref_nalu_size = data_length;
					}

					if (ref_nalu_size > mdia->tmp_nal_copy_buffer_alloc) {
						mdia->tmp_nal_copy_buffer_alloc = ref_nalu_size;
						mdia->tmp_nal_copy_buffer = (char*) gf_realloc(mdia->tmp_nal_copy_buffer, sizeof(char) * ref_nalu_size );
					}
					gf_bs_read_data(mdia->extracted_bs, mdia->tmp_nal_copy_buffer, ref_nalu_size);

					if (!header_written) {
						if (rewrite_start_codes)
							gf_bs_write_u32(mdia->nalu_out_bs, 1);
						else
							gf_bs_write_int(mdia->nalu_out_bs, ref_nalu_size, 8*nal_unit_size_field);
					}
					assert(data_length >= ref_nalu_size);
					gf_bs_write_data(mdia->nalu_out_bs, mdia->tmp_nal_copy_buffer, ref_nalu_size);
					data_length -= ref_nalu_size;

					header_written = GF_FALSE;

				}
			} else {
				GF_LOG(GF_LOG_ERROR, GF_LOG_CONTAINER, ("ISOBMF: Extractor size is larger than referred sample size - skipping.\n"));
			}
			gf_isom_set_nalu_extract_mode(file, ref_track_num, cur_extract_mode);

			if (!is_hevc) break;
		}
		break;
	case 1:
		//skip to end of this NALU
		gf_bs_skip_bytes(mdia->nalu_parser, nal_size - nb_bytes_nalh);
		break;
	case 2:
		if (nal_size - nb_bytes_nalh > mdia->tmp_nal_copy_buffer_alloc) {
			mdia->tmp_nal_copy_buffer_alloc = nal_size - nb_bytes_nalh;
			mdia->tmp_nal_copy_buffer = (char*) gf_realloc(mdia->tmp_nal_copy_buffer, sizeof(char) * (nal_size - nb_bytes_nalh) );
		}
		gf_bs_read_data(mdia->nalu_parser, mdia->tmp_nal_copy_buffer, nal_size - nb_bytes_nalh);
		if (rewrite_start_codes)
			gf_bs_write_u32(mdia->nalu_out_bs, 1);
		else
			gf_bs_write_int(mdia->nalu_out_bs, nal_size, 8*nal_unit_size_field);

		gf_bs_write_u8(mdia->nalu_out_bs, nal_hdr);
		gf_bs_write_data(mdia->nalu_out_bs, mdia->tmp_nal_copy_buffer, nal_size - nb_bytes_nalh);
		break;
	}
	return GF_OK;
}