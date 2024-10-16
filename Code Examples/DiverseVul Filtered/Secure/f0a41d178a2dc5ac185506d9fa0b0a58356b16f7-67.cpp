GF_Err gf_isom_get_sample_to_group_info(GF_ISOFile *the_file, u32 trackNumber, u32 sample_number, u32 grouping_type, u32 grouping_type_param, u32 *sampleGroupDescIndex)
{
	GF_TrackBox *trak;
	u32 i, count;

	if (!grouping_type || !sampleGroupDescIndex) return GF_BAD_PARAM;

	*sampleGroupDescIndex = 0;

	trak = gf_isom_get_track_from_file(the_file, trackNumber);
	if (!trak) return GF_BAD_PARAM;
	if (!trak->Media->information->sampleTable->sampleGroups) return GF_OK;

#ifndef	GPAC_DISABLE_ISOM_FRAGMENTS
	if (sample_number <= trak->sample_count_at_seg_start) return GF_BAD_PARAM;
	sample_number -= trak->sample_count_at_seg_start;
#endif

	count = gf_list_count(trak->Media->information->sampleTable->sampleGroups);
	for (i=0; i<count; i++) {
		GF_SampleGroupBox *sg;
		u32 j;
		u32 first_sample_in_entry, last_sample_in_entry;
		first_sample_in_entry = 1;
		sg = (GF_SampleGroupBox*)gf_list_get(trak->Media->information->sampleTable->sampleGroups, i);
		if (sg->grouping_type != grouping_type) continue;
		if (sg->grouping_type_parameter != grouping_type_param) continue;

		for (j=0; j<sg->entry_count; j++) {
			last_sample_in_entry = first_sample_in_entry + sg->sample_entries[j].sample_count - 1;
			if ((sample_number<first_sample_in_entry) || (sample_number>last_sample_in_entry)) {
				first_sample_in_entry = last_sample_in_entry+1;
				continue;
			}
			/*we found our sample*/
			*sampleGroupDescIndex = sg->sample_entries[j].group_description_index;
			return GF_OK;
		}
	}
	return GF_OK;
}