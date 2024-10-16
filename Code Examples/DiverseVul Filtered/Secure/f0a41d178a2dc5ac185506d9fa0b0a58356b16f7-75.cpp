GF_DefaultSampleGroupDescriptionEntry * gf_isom_get_sample_group_info_entry(GF_ISOFile *the_file, GF_TrackBox *trak, u32 grouping_type, u32 sample_group_description_index, u32 *default_index, GF_SampleGroupDescriptionBox **out_sgdp)
{
	u32 i, count;

	if (!trak || !sample_group_description_index) return NULL;
	if (!trak->Media->information->sampleTable->sampleGroupsDescription) return NULL;

	count = gf_list_count(trak->Media->information->sampleTable->sampleGroupsDescription);
	for (i=0; i<count; i++) {
		GF_SampleGroupDescriptionBox *sgdesc = (GF_SampleGroupDescriptionBox*)gf_list_get(trak->Media->information->sampleTable->sampleGroupsDescription, i);
		if (sgdesc->grouping_type != grouping_type) continue;

		if (sgdesc->default_description_index && !sample_group_description_index) sample_group_description_index = sgdesc->default_description_index;

		if (default_index) *default_index = sgdesc->default_description_index ;
		if (out_sgdp) *out_sgdp = sgdesc;

		if (!sample_group_description_index) return NULL;
		return (GF_DefaultSampleGroupDescriptionEntry*)gf_list_get(sgdesc->group_descriptions, sample_group_description_index-1);
	}
	return NULL;
}