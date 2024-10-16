GF_Err gf_isom_remove_cenc_saio(GF_ISOFile *the_file, u32 trackNumber)
{
	u32 i;
	GF_SampleTableBox *stbl;
	GF_TrackBox *trak = gf_isom_get_track_from_file(the_file, trackNumber);
	if (!trak) return GF_BAD_PARAM;

	stbl = trak->Media->information->sampleTable;
	if (!stbl)
		return GF_BAD_PARAM;

	for (i = 0; i < gf_list_count(stbl->sai_offsets); i++) {
		GF_SampleAuxiliaryInfoOffsetBox *saio = (GF_SampleAuxiliaryInfoOffsetBox *)gf_list_get(stbl->sai_offsets, i);
		switch (saio->aux_info_type) {
		case GF_ISOM_CENC_SCHEME:
		case GF_ISOM_CENS_SCHEME:
		case GF_ISOM_CBC_SCHEME:
		case GF_ISOM_CBCS_SCHEME:
		case 0:
			break;
		default:
			continue;
		}
		gf_isom_box_del_parent(&stbl->child_boxes, (GF_Box *)saio);
		gf_list_rem(stbl->sai_offsets, i);
		i--;
	}

	if (!gf_list_count(stbl->sai_offsets)) {
		gf_list_del(stbl->sai_offsets);
		stbl->sai_offsets = NULL;
	}

	return GF_OK;
}