Bool gf_isom_has_cenc_sample_group(GF_ISOFile *the_file, u32 trackNumber)
{
	GF_TrackBox *trak;
	u32 i, count;

	trak = gf_isom_get_track_from_file(the_file, trackNumber);
	if (!trak) return GF_FALSE;
	if (!trak->Media->information->sampleTable->sampleGroups) return GF_FALSE;

	count = gf_list_count(trak->Media->information->sampleTable->sampleGroupsDescription);
	for (i=0; i<count; i++) {
		GF_SampleGroupDescriptionBox *sgdesc = (GF_SampleGroupDescriptionBox*)gf_list_get(trak->Media->information->sampleTable->sampleGroupsDescription, i);
		if (sgdesc->grouping_type==GF_ISOM_SAMPLE_GROUP_SEIG) {
			return GF_TRUE;
		}
	}
	return GF_FALSE;
}