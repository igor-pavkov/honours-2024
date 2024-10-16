GF_Err senc_Parse(GF_BitStream *bs, GF_TrackBox *trak, void *traf, GF_SampleEncryptionBox *senc)
#endif
{
	GF_Err e;
	Bool parse_failed = GF_FALSE;
	u32 i, count, sample_number;
	u32 senc_size = (u32) senc->size;
	u32 subs_size = 0, def_IV_size;
	u64 pos = gf_bs_get_position(bs);
	Bool do_warn = GF_TRUE;
	Bool use_multikey = GF_FALSE;

#ifdef	GPAC_DISABLE_ISOM_FRAGMENTS
	if (!traf)
		return GF_BAD_PARAM;
#endif

	//BOX + version/flags
	if (senc_size<12) return GF_BAD_PARAM;
	senc_size -= 12;

	if (senc->piff_type==1) {
		//UUID
		if (senc_size<16) return GF_BAD_PARAM;
		senc_size -= 16;
	} else if (!senc->piff_type) {
		if (senc->version==1)
			use_multikey = GF_TRUE;
	}
	if (senc->flags & 2) subs_size = 8;

	if (senc_size<4) return GF_BAD_PARAM;

	sample_number = 1;
#ifndef	GPAC_DISABLE_ISOM_FRAGMENTS
	if (trak) sample_number += trak->sample_count_at_seg_start;
#endif

	gf_bs_seek(bs, senc->bs_offset);

	count = gf_bs_read_u32(bs);
	senc_size -= 4;

	def_IV_size = 0;
	//check the target size if we have one subsample
	if (senc_size >= count * (16 + subs_size)) {
		def_IV_size = 16;
	}
	else if (senc_size >= count * (8 + subs_size)) {
		def_IV_size = 8;
	}
	else if (senc_size >= count * (subs_size)) {
		def_IV_size = 0;
	}

	if (!senc->samp_aux_info) senc->samp_aux_info = gf_list_new();
	for (i=0; i<count; i++) {
		const u8 *key_info=NULL;
		u32 key_info_size=0;
		Bool is_encrypted;
		GF_CENCSampleAuxInfo *sai;
		u8 IV_size=0;
		u32 nb_keys = 0;
		u32 nb_bytes_subsample = 6;
		u32 nb_subs_bits = 16;

		GF_SAFEALLOC(sai, GF_CENCSampleAuxInfo);
		if (!sai) {
			gf_bs_seek(bs, pos);
			return GF_OUT_OF_MEM;
		}
		if (trak) {
			e = gf_isom_get_sample_cenc_info_internal(trak, traf, senc, sample_number, &is_encrypted, NULL, NULL, &key_info, &key_info_size);
			if (! key_info) {
				IV_size = key_info_size; //piff default
				use_multikey = GF_FALSE;
				senc->piff_type = 2;
			} else if (use_multikey) {
				nb_keys = key_info[1];
				nb_keys <<= 8;
				nb_keys |= key_info[2];
				nb_bytes_subsample = 8;
				nb_subs_bits = 32;
			} else {
				IV_size = key_info[3];
			}

			if (e) {
				GF_LOG(GF_LOG_ERROR, GF_LOG_CONTAINER, ("[isobmf] could not get cenc info for sample %d: %s\n", sample_number, gf_error_to_string(e) ));
				gf_isom_cenc_samp_aux_info_del(sai);
				gf_bs_seek(bs, pos);
				if (trak->moov->mov->FragmentsFlags & GF_ISOM_FRAG_READ_DEBUG)
					return GF_OK;
				return e;
			}
		}
		//no init movie setup (segment dump/inspaction, assume default encrypted and 16 bytes IV
		else {
			if (do_warn) {
				GF_LOG(GF_LOG_WARNING, GF_LOG_CONTAINER, ("[isobmf] no moov found, cannot get cenc default info, assuming isEncrypted, IV size %d (computed from senc size)\n", def_IV_size));
				do_warn = GF_FALSE;
			}
			is_encrypted = GF_TRUE;
			IV_size = def_IV_size;
		}
		if (senc_size < IV_size) {
			parse_failed = GF_TRUE;
			gf_isom_cenc_samp_aux_info_del(sai);
			break;
		}

		sample_number++;

		//subsample info is only signaled for encrypted samples
		if (is_encrypted) {
			u64 sai_start = gf_bs_get_position(bs);
			u32 nb_subs = 0;

			if (use_multikey) {
				u32 j;
				u32 nb_iv_init = gf_bs_read_u16(bs);
				for (j=0; j<nb_iv_init; j++) {
					u32 idx = gf_bs_read_u16(bs);
					IV_size = key_info_get_iv_size(key_info, nb_keys, idx, NULL, NULL);
					if (!IV_size) {
						gf_isom_cenc_samp_aux_info_del(sai);
						GF_LOG(GF_LOG_WARNING, GF_LOG_CONTAINER, ("[isobmf] Failed to parse SENC box, invalid SAI multikey with IV size 0\n" ));
						gf_bs_seek(bs, pos);
						return GF_ISOM_INVALID_FILE;
					}
					gf_bs_skip_bytes(bs, IV_size);
				}

			} else {
				if (IV_size > 16) {
					gf_isom_cenc_samp_aux_info_del(sai);
					GF_LOG(GF_LOG_WARNING, GF_LOG_CONTAINER, ("[isobmf] Failed to parse SENC box, invalid SAI size\n" ));
					gf_bs_seek(bs, pos);
					return GF_ISOM_INVALID_FILE;
				}
				if (IV_size) {
					gf_bs_skip_bytes(bs, IV_size);
				}
			}
			if (senc->flags & 0x00000002) {
				nb_subs = gf_bs_read_int(bs, nb_subs_bits);
			}

			sai->cenc_data_size = (u32) (gf_bs_get_position(bs) - sai_start);
			sai->cenc_data_size += nb_subs * nb_bytes_subsample;
			gf_bs_seek(bs, sai_start);

			if ((s32) senc_size < sai->cenc_data_size) {
				parse_failed = GF_TRUE;
				gf_isom_cenc_samp_aux_info_del(sai);
				break;
			}

			sai->cenc_data = gf_malloc(sizeof(u8) * sai->cenc_data_size);
			if (!sai->cenc_data) {
				gf_isom_cenc_samp_aux_info_del(sai);
				gf_bs_seek(bs, pos);
				return GF_OUT_OF_MEM;
			}
			gf_bs_read_data(bs, sai->cenc_data, sai->cenc_data_size);
			senc_size -= sai->cenc_data_size;
		} else {
			i--;
			sai->isNotProtected = 1;
		}

		if (senc->internal_4cc == GF_ISOM_BOX_UUID_PSEC) {
			sai->key_info_size = IV_size;
		} else {
			sai->key_info = key_info;
			sai->key_info_size = key_info_size;
		}
		gf_list_add(senc->samp_aux_info, sai);
	}
	gf_bs_seek(bs, pos);
	if (parse_failed) {
		GF_LOG(GF_LOG_ERROR, GF_LOG_CONTAINER, ("[isobmf] cannot parse senc, missing IV/crypto state\n"));
	}
	return GF_OK;
}