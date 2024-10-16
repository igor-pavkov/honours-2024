void sgpd_write_entry(u32 grouping_type, void *entry, GF_BitStream *bs)
{
	switch (grouping_type) {
	case GF_ISOM_SAMPLE_GROUP_ROLL:
	case GF_ISOM_SAMPLE_GROUP_PROL:
		gf_bs_write_int(bs, ((GF_RollRecoveryEntry*)entry)->roll_distance, 16);
		return;
	case GF_ISOM_SAMPLE_GROUP_RAP:
		gf_bs_write_int(bs, ((GF_VisualRandomAccessEntry*)entry)->num_leading_samples_known, 1);
		gf_bs_write_int(bs, ((GF_VisualRandomAccessEntry*)entry)->num_leading_samples, 7);
		return;
	case GF_ISOM_SAMPLE_GROUP_SAP:
		gf_bs_write_int(bs, ((GF_SAPEntry*)entry)->dependent_flag, 1);
		gf_bs_write_int(bs, 0, 3);
		gf_bs_write_int(bs, ((GF_SAPEntry*)entry)->SAP_type, 4);
		return;
	case GF_ISOM_SAMPLE_GROUP_SYNC:
		gf_bs_write_int(bs, 0, 2);
		gf_bs_write_int(bs, ((GF_SYNCEntry*)entry)->NALU_type, 6);
		return;
	case GF_ISOM_SAMPLE_GROUP_TELE:
		gf_bs_write_int(bs, ((GF_TemporalLevelEntry*)entry)->level_independently_decodable, 1);
		gf_bs_write_int(bs, 0, 7);
		return;
	case GF_ISOM_SAMPLE_GROUP_SEIG:
	{
		GF_CENCSampleEncryptionGroupEntry *seig = (GF_CENCSampleEncryptionGroupEntry *)entry;
		Bool use_mkey = seig->key_info[0];
		u32 nb_keys = 1;
		if (use_mkey) {
			nb_keys = seig->key_info[1];
			nb_keys<<=8;
			nb_keys |= seig->key_info[2];
		}
		gf_bs_write_int(bs, use_mkey ? 1 : 0, 1);
		gf_bs_write_int(bs, 0, 7);
		gf_bs_write_int(bs, seig->crypt_byte_block, 4);
		gf_bs_write_int(bs, seig->skip_byte_block, 4);
		gf_bs_write_u8(bs, seig->IsProtected);
		if (nb_keys>1) {
			gf_bs_write_data(bs, seig->key_info+1, seig->key_info_size-1);
		} else {
			gf_bs_write_data(bs, seig->key_info+3, seig->key_info_size - 3);
		}
	}
		return;
	case GF_ISOM_SAMPLE_GROUP_OINF:
		gf_isom_oinf_write_entry(entry, bs);
		return;
	case GF_ISOM_SAMPLE_GROUP_LINF:
		gf_isom_linf_write_entry(entry, bs);
		return;

	case GF_ISOM_SAMPLE_GROUP_SPOR:
	{
		u32 i;
		GF_SubpictureOrderEntry *spor = (GF_SubpictureOrderEntry *) entry;
		gf_bs_write_int(bs, spor->subpic_id_info_flag, 1);
		gf_bs_write_int(bs, spor->num_subpic_ref_idx, 15);
		for (i=0; i<spor->num_subpic_ref_idx; i++) {
			gf_bs_write_u16(bs, spor->subp_track_ref_idx[i]);
		}
		if (spor->subpic_id_info_flag) {
			gf_bs_write_int(bs, spor->spinfo.subpic_id_len_minus1, 4);
			gf_bs_write_int(bs, spor->spinfo.subpic_id_bit_pos, 12);
			gf_bs_write_int(bs, spor->spinfo.start_code_emul_flag, 1);
			gf_bs_write_int(bs, spor->spinfo.pps_sps_subpic_id_flag, 1);
			if (spor->spinfo.pps_sps_subpic_id_flag) {
				gf_bs_write_int(bs, spor->spinfo.xps_id, 6);
			} else {
				gf_bs_write_int(bs, spor->spinfo.xps_id, 4);
				gf_bs_write_int(bs, 0, 2);
			}
		}
		return;
	}

	case GF_ISOM_SAMPLE_GROUP_SULM:
	{
		u32 i;
		GF_SubpictureLayoutMapEntry *sulm = (GF_SubpictureLayoutMapEntry *) entry;
		gf_bs_write_u32(bs, sulm->groupID_info_4cc);
		gf_bs_write_u16(bs, sulm->nb_entries - 1);
		for (i=0; i<sulm->nb_entries; i++) {
			gf_bs_write_u16(bs, sulm->groupIDs[i]);
		}
		return;
	}

	default:
	{
		GF_DefaultSampleGroupDescriptionEntry *ptr = (GF_DefaultSampleGroupDescriptionEntry *)entry;
		if (ptr->length)
			gf_bs_write_data(bs, (char *) ptr->data, ptr->length);
	}
	}
}