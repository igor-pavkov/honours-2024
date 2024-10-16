static u32 sgpd_size_entry(u32 grouping_type, void *entry)
{
	switch (grouping_type) {
	case GF_ISOM_SAMPLE_GROUP_ROLL:
	case GF_ISOM_SAMPLE_GROUP_PROL:
		return 2;
	case GF_ISOM_SAMPLE_GROUP_TELE:
	case GF_ISOM_SAMPLE_GROUP_RAP:
	case GF_ISOM_SAMPLE_GROUP_SAP:
	case GF_ISOM_SAMPLE_GROUP_SYNC:
		return 1;
	case GF_ISOM_SAMPLE_GROUP_TSCL:
		return 20;
	case GF_ISOM_SAMPLE_GROUP_LBLI:
		return 2;
	case GF_ISOM_SAMPLE_GROUP_TSAS:
	case GF_ISOM_SAMPLE_GROUP_STSA:
		return 0;
	case GF_ISOM_SAMPLE_GROUP_SEIG:
	{
		GF_CENCSampleEncryptionGroupEntry *seig = (GF_CENCSampleEncryptionGroupEntry *)entry;
		Bool use_mkey = seig->key_info[0] ? GF_TRUE : GF_FALSE;
		if (use_mkey) {
			return 3 + seig->key_info_size-1;
		}
		return seig->key_info_size; //== 3 + (seig->key_info_size-3);
	}
	case GF_ISOM_SAMPLE_GROUP_OINF:
		return gf_isom_oinf_size_entry(entry);
	case GF_ISOM_SAMPLE_GROUP_LINF:
		return gf_isom_linf_size_entry(entry);
	case GF_ISOM_SAMPLE_GROUP_SPOR:
	{
		GF_SubpictureOrderEntry *spor = (GF_SubpictureOrderEntry *)entry;
		u32 s = 2 + 2*spor->num_subpic_ref_idx;
		if (spor->subpic_id_info_flag) {
			s += 3;
		}
		return s;
	}
	case GF_ISOM_SAMPLE_GROUP_SULM:
	{
		GF_SubpictureLayoutMapEntry *sulm = (GF_SubpictureLayoutMapEntry *) entry;
		return 6 + 2*sulm->nb_entries;
	}

	default:
		return ((GF_DefaultSampleGroupDescriptionEntry *)entry)->length;
	}
}