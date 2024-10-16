Bool gf_isom_get_sample_group_info(GF_ISOFile *the_file, u32 trackNumber, u32 sample_description_index, u32 grouping_type, u32 *default_index, const u8 **data, u32 *size)
{
	GF_DefaultSampleGroupDescriptionEntry *sg_entry;
	GF_TrackBox *trak = gf_isom_get_track_from_file(the_file, trackNumber);

	if (default_index) *default_index = 0;
	if (size) *size = 0;
	if (data) *data = NULL;

	sg_entry = gf_isom_get_sample_group_info_entry(the_file, trak, grouping_type, sample_description_index, default_index, NULL);
	if (!sg_entry) return GF_FALSE;

	switch (grouping_type) {
	case GF_ISOM_SAMPLE_GROUP_RAP:
	case GF_ISOM_SAMPLE_GROUP_SYNC:
	case GF_ISOM_SAMPLE_GROUP_ROLL:
	case GF_ISOM_SAMPLE_GROUP_SEIG:
	case GF_ISOM_SAMPLE_GROUP_OINF:
	case GF_ISOM_SAMPLE_GROUP_LINF:
		return GF_TRUE;
	default:
		if (sg_entry && data) *data = (char *) sg_entry->data;
		if (sg_entry && size) *size = sg_entry->length;
		return GF_TRUE;
	}
	return GF_FALSE;
}