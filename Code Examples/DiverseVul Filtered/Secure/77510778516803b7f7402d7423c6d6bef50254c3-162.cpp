static void *sgpd_parse_entry(u32 grouping_type, GF_BitStream *bs, s32 bytes_in_box, u32 entry_size, u32 *total_bytes)
{
	Bool null_size_ok = GF_FALSE;
	GF_DefaultSampleGroupDescriptionEntry *def_ptr;

	switch (grouping_type) {
	case GF_ISOM_SAMPLE_GROUP_ROLL:
	case GF_ISOM_SAMPLE_GROUP_PROL:
	{
		GF_RollRecoveryEntry *ptr;
		GF_SAFEALLOC(ptr, GF_RollRecoveryEntry);
		if (!ptr) return NULL;
		ptr->roll_distance = gf_bs_read_int(bs, 16);
		*total_bytes = 2;
		return ptr;
	}
	case GF_ISOM_SAMPLE_GROUP_RAP:
	{
		GF_VisualRandomAccessEntry *ptr;
		GF_SAFEALLOC(ptr, GF_VisualRandomAccessEntry);
		if (!ptr) return NULL;
		ptr->num_leading_samples_known = gf_bs_read_int(bs, 1);
		ptr->num_leading_samples = gf_bs_read_int(bs, 7);
		*total_bytes = 1;
		return ptr;
	}
	case GF_ISOM_SAMPLE_GROUP_SAP:
	{
		GF_SAPEntry *ptr;
		GF_SAFEALLOC(ptr, GF_SAPEntry);
		if (!ptr) return NULL;
		ptr->dependent_flag = gf_bs_read_int(bs, 1);
		gf_bs_read_int(bs, 3);
		ptr->SAP_type = gf_bs_read_int(bs, 4);
		*total_bytes = 1;
		return ptr;
	}
	case GF_ISOM_SAMPLE_GROUP_SYNC:
	{
		GF_SYNCEntry *ptr;
		GF_SAFEALLOC(ptr, GF_SYNCEntry);
		if (!ptr) return NULL;
		gf_bs_read_int(bs, 2);
		ptr->NALU_type = gf_bs_read_int(bs, 6);
		*total_bytes = 1;
		return ptr;
	}
	case GF_ISOM_SAMPLE_GROUP_TELE:
	{
		GF_TemporalLevelEntry *ptr;
		GF_SAFEALLOC(ptr, GF_TemporalLevelEntry);
		if (!ptr) return NULL;
		ptr->level_independently_decodable = gf_bs_read_int(bs, 1);
		gf_bs_read_int(bs, 7);
		*total_bytes = 1;
		return ptr;
	}
	case GF_ISOM_SAMPLE_GROUP_SEIG:
	{
		GF_CENCSampleEncryptionGroupEntry *ptr;
		if (bytes_in_box<3) return NULL;
		GF_SAFEALLOC(ptr, GF_CENCSampleEncryptionGroupEntry);
		if (!ptr) return NULL;
		Bool use_mkey = gf_bs_read_int(bs, 1);
		gf_bs_read_int(bs, 7); //reserved
		ptr->crypt_byte_block = gf_bs_read_int(bs, 4);
		ptr->skip_byte_block = gf_bs_read_int(bs, 4);
		ptr->IsProtected = gf_bs_read_u8(bs);
		bytes_in_box -= 3;
		if (use_mkey) {
			u64 pos = gf_bs_get_position(bs);
			u32 i, count = gf_bs_read_u16(bs);
			bytes_in_box -= 2;
			if (bytes_in_box<0) {
				gf_free(ptr);
				return NULL;
			}
			for (i=0; i<count; i++) {
				u8 ivsize = gf_bs_read_u8(bs);
				gf_bs_skip_bytes(bs, 16);
				bytes_in_box -= 17;
				if (!ivsize) {
					//const IV
					ivsize = gf_bs_read_u8(bs);
					gf_bs_skip_bytes(bs, ivsize);
					bytes_in_box -= 1 + ivsize;
				}
				if (bytes_in_box<0) {
					gf_free(ptr);
					return NULL;
				}
			}
			ptr->key_info_size = 1 + (u32) (gf_bs_get_position(bs) - pos);
			ptr->key_info = gf_malloc(sizeof(u8) * ptr->key_info_size);
			if (!ptr->key_info) {
				gf_free(ptr);
				return NULL;
			}
			gf_bs_seek(bs, pos);
			ptr->key_info[0] = 1;
			gf_bs_read_data(bs, ptr->key_info + 1, ptr->key_info_size - 1);
			*total_bytes = 3 + ptr->key_info_size - 1;

			if (!gf_cenc_validate_key_info(ptr->key_info, ptr->key_info_size)) {
				gf_free(ptr->key_info);
				gf_free(ptr);
				return NULL;
			}
		} else {
			bin128 kid;
			u8 const_iv_size = 0;
			u8 iv_size = gf_bs_read_u8(bs);
			gf_bs_read_data(bs, kid, 16);
			bytes_in_box -= 17;
			if (bytes_in_box<0) {
				gf_free(ptr);
				return NULL;
			}

			*total_bytes = 20;
			if ((ptr->IsProtected == 1) && !iv_size) {
				const_iv_size = gf_bs_read_u8(bs);
				if ((const_iv_size != 8) && (const_iv_size != 16)) {
					GF_LOG(GF_LOG_WARNING, GF_LOG_CONTAINER, ("[iso file] seig sample group have invalid constant_IV size\n"));
					gf_free(ptr);
					return NULL;
				}
			}
			ptr->key_info_size = 20;
			if (!iv_size && ptr->IsProtected) {
				ptr->key_info_size += 1 + const_iv_size;
			}
			ptr->key_info = gf_malloc(sizeof(u8) * ptr->key_info_size);
			if (!ptr->key_info) {
				gf_free(ptr);
				return NULL;
			}
			ptr->key_info[0] = 0;
			ptr->key_info[1] = 0;
			ptr->key_info[2] = 0;
			ptr->key_info[3] = iv_size;
			memcpy(ptr->key_info+4, kid, 16);
			if (!iv_size && ptr->IsProtected) {
				ptr->key_info[20] = const_iv_size;
				gf_bs_read_data(bs, (char *)ptr->key_info+21, const_iv_size);
				*total_bytes += 1 + const_iv_size;
			}
		}

		if (!entry_size) {
			GF_LOG(GF_LOG_WARNING, GF_LOG_CONTAINER, ("[iso file] seig sample group does not indicate entry size, deprecated in spec\n"));
		}
		return ptr;
	}
	case GF_ISOM_SAMPLE_GROUP_OINF:
	{
		GF_OperatingPointsInformation *ptr = gf_isom_oinf_new_entry();
		u32 s = (u32) gf_bs_get_position(bs);
		gf_isom_oinf_read_entry(ptr, bs);
		*total_bytes = (u32) gf_bs_get_position(bs) - s;
		if (!entry_size) {
			GF_LOG(GF_LOG_WARNING, GF_LOG_CONTAINER, ("[iso file] oinf sample group does not indicate entry size, deprecated in spec\n"));
		}
		return ptr;
	}
	case GF_ISOM_SAMPLE_GROUP_LINF:
	{
		GF_LHVCLayerInformation *ptr = gf_isom_linf_new_entry();
		u32 s = (u32) gf_bs_get_position(bs);
		gf_isom_linf_read_entry(ptr, bs);
		*total_bytes = (u32) gf_bs_get_position(bs) - s;
		if (!entry_size) {
			GF_LOG(GF_LOG_WARNING, GF_LOG_CONTAINER, ("[iso file] linf sample group does not indicate entry size, deprecated in spec\n"));
		}
		return ptr;
	}

	case GF_ISOM_SAMPLE_GROUP_TRIF:
		if (! entry_size) {
			u32 flags = gf_bs_peek_bits(bs, 24, 0);
			if (flags & 0x10000) entry_size=3;
			else {
				if (flags & 0x80000) entry_size=7;
				else entry_size=11;
				//have dependency list
				if (flags & 0x200000) {
					u32 nb_entries = gf_bs_peek_bits(bs, 16, entry_size);
					entry_size += 2 + 2*nb_entries;
				}
			}
			GF_LOG(GF_LOG_WARNING, GF_LOG_CONTAINER, ("[iso file] trif sample group does not indicate entry size, deprecated in spec\n"));
		}
		break;
	case GF_ISOM_SAMPLE_GROUP_NALM:
		if (! entry_size) {
			u64 start = gf_bs_get_position(bs);
			Bool rle, large_size;
			u32 entry_count;
			gf_bs_read_int(bs, 6);
			large_size = gf_bs_read_int(bs, 1);
			rle = gf_bs_read_int(bs, 1);
			entry_count = gf_bs_read_int(bs, large_size ? 16 : 8);
			gf_bs_seek(bs, start);
			entry_size = 1 + (large_size ? 2 : 1);
			entry_size += entry_count * 2;
			if (rle) entry_size += entry_count * (large_size ? 2 : 1);
			GF_LOG(GF_LOG_WARNING, GF_LOG_CONTAINER, ("[iso file] nalm sample group does not indicate entry size, deprecated in spec\n"));
		}
		break;

	case GF_ISOM_SAMPLE_GROUP_TSAS:
	case GF_ISOM_SAMPLE_GROUP_STSA:
		null_size_ok = GF_TRUE;
		break;
	//TODO, add support for these ones ?
	case GF_ISOM_SAMPLE_GROUP_TSCL:
		entry_size = 20;
		break;
	case GF_ISOM_SAMPLE_GROUP_LBLI:
		entry_size = 2;
		break;
	case GF_ISOM_SAMPLE_GROUP_SPOR:
	{
		u32 i;
		GF_SubpictureOrderEntry *ptr;
		GF_SAFEALLOC(ptr, GF_SubpictureOrderEntry);
		if (!ptr) return NULL;
		ptr->subpic_id_info_flag = gf_bs_read_int(bs, 1);
		ptr->num_subpic_ref_idx = gf_bs_read_int(bs, 15);
		*total_bytes = 2;
		ptr->subp_track_ref_idx = gf_malloc(sizeof(u16) * ptr->num_subpic_ref_idx);
		if (!ptr->subp_track_ref_idx) {
			gf_free(ptr);
			return NULL;
		}
		for (i=0; i<ptr->num_subpic_ref_idx; i++) {
			ptr->subp_track_ref_idx[i] = gf_bs_read_u16(bs);
			*total_bytes += 2;
		}
		if (ptr->subpic_id_info_flag) {
			ptr->spinfo.subpic_id_len_minus1 = gf_bs_read_int(bs, 4);
			ptr->spinfo.subpic_id_bit_pos = gf_bs_read_int(bs, 12);
			ptr->spinfo.start_code_emul_flag = gf_bs_read_int(bs, 1);
			ptr->spinfo.pps_sps_subpic_id_flag = gf_bs_read_int(bs, 1);
			if (ptr->spinfo.pps_sps_subpic_id_flag) {
				ptr->spinfo.xps_id = gf_bs_read_int(bs, 6);
			} else {
				ptr->spinfo.xps_id = gf_bs_read_int(bs, 4);
				gf_bs_read_int(bs, 2);
			}
			*total_bytes += 3;
		}
		return ptr;
	}
	case GF_ISOM_SAMPLE_GROUP_SULM:
	{
		u32 i;
		GF_SubpictureLayoutMapEntry *ptr;
		GF_SAFEALLOC(ptr, GF_SubpictureLayoutMapEntry);
		if (!ptr) return NULL;
		ptr->groupID_info_4cc = gf_bs_read_u32(bs);
		ptr->nb_entries = 1 + gf_bs_read_u16(bs);
		*total_bytes = 6;
		ptr->groupIDs = gf_malloc(sizeof(u16) * ptr->nb_entries);
		if (!ptr->groupIDs) {
			gf_free(ptr);
			return NULL;
		}
		for (i=0; i<ptr->nb_entries; i++) {
			ptr->groupIDs[i] = gf_bs_read_u16(bs);
			*total_bytes += 2;
		}
		return ptr;
	}
	default:
		break;
	}

	if (!entry_size && !null_size_ok) {
		GF_LOG(GF_LOG_WARNING, GF_LOG_CONTAINER, ("[iso file] %s sample group does not indicate entry size and is not implemented, cannot parse!\n", gf_4cc_to_str( grouping_type) ));
		return NULL;
	}
	GF_SAFEALLOC(def_ptr, GF_DefaultSampleGroupDescriptionEntry);
	if (!def_ptr) return NULL;
	if (entry_size) {
		def_ptr->length = entry_size;
		def_ptr->data = (u8 *) gf_malloc(sizeof(u8)*def_ptr->length);
		if (!def_ptr->data) {
			gf_free(def_ptr);
			return NULL;
		}
		gf_bs_read_data(bs, (char *) def_ptr->data, def_ptr->length);
		*total_bytes = entry_size;
	}
	return def_ptr;
}