GF_Err gf_isom_remove_samp_group_box(GF_ISOFile *the_file, u32 trackNumber)
{
	u32 i;
	GF_SampleTableBox *stbl;
	GF_TrackBox *trak = gf_isom_get_track_from_file(the_file, trackNumber);
	if (!trak) return GF_BAD_PARAM;
	stbl = trak->Media->information->sampleTable;
	if (!stbl)
		return GF_BAD_PARAM;

	for (i = 0; i < gf_list_count(stbl->sampleGroupsDescription); i++) {
		GF_SampleGroupDescriptionBox *a = (GF_SampleGroupDescriptionBox *)gf_list_get(stbl->sampleGroupsDescription, i);
		if (a->grouping_type == GF_ISOM_SAMPLE_GROUP_SEIG) {
			gf_list_rem(stbl->sampleGroupsDescription, i);
			gf_isom_box_del_parent(&stbl->child_boxes, (GF_Box *) a);
			i--;
		}
	}
	if (!gf_list_count(stbl->sampleGroupsDescription)) {
		gf_list_del(stbl->sampleGroupsDescription);
		stbl->sampleGroupsDescription = NULL;
	}

	for (i = 0; i < gf_list_count(stbl->sampleGroups); i++) {
		GF_SampleGroupBox *a = (GF_SampleGroupBox *)gf_list_get(stbl->sampleGroups, i);
		if (a->grouping_type == GF_ISOM_SAMPLE_GROUP_SEIG) {
			gf_list_rem(stbl->sampleGroups, i);
			gf_isom_box_del_parent(&stbl->child_boxes, (GF_Box *) a);
			i--;
		}
	}
	if (!gf_list_count(stbl->sampleGroups)) {
		gf_list_del(stbl->sampleGroups);
		stbl->sampleGroups = NULL;
	}

	return GF_OK;
}