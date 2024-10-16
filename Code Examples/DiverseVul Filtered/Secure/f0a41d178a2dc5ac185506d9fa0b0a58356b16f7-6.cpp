GF_Err gf_isom_enum_sample_group(GF_ISOFile *the_file, u32 trackNumber, u32 sample_number, u32 *sgrp_idx, u32 *sgrp_type, u32 *sgrp_parameter, const u8 **sgrp_data, u32 *sgrp_size)
{
	GF_TrackBox *trak;
	u32 i, count;

	if (!sgrp_idx || !sgrp_type) return GF_BAD_PARAM;
	if (sgrp_parameter) *sgrp_parameter = 0;
	if (sgrp_data) *sgrp_data = NULL;
	if (sgrp_size) *sgrp_size = 0;
	*sgrp_type = 0;

	trak = gf_isom_get_track_from_file(the_file, trackNumber);
	if (!trak) return GF_BAD_PARAM;
	if (!trak->Media->information->sampleTable->sampleGroups) return GF_OK;

#ifndef	GPAC_DISABLE_ISOM_FRAGMENTS
	if (sample_number <= trak->sample_count_at_seg_start) return GF_BAD_PARAM;
	sample_number -= trak->sample_count_at_seg_start;
#endif

	count = gf_list_count(trak->Media->information->sampleTable->sampleGroupsDescription);
	for (i=0; i<count; i++) {
		GF_SampleGroupBox *sg;
		u32 j;
		GF_SampleGroupDescriptionBox *sgd = (GF_SampleGroupDescriptionBox*)gf_list_get(trak->Media->information->sampleTable->sampleGroupsDescription, i);

		switch (sgd->grouping_type) {
		case GF_ISOM_SAMPLE_GROUP_ROLL:
		case GF_ISOM_SAMPLE_GROUP_PROL:
		case GF_ISOM_SAMPLE_GROUP_TELE:
		case GF_ISOM_SAMPLE_GROUP_RAP:
		case GF_ISOM_SAMPLE_GROUP_SYNC:
		case GF_ISOM_SAMPLE_GROUP_SEIG:
		case GF_ISOM_SAMPLE_GROUP_OINF:
		case GF_ISOM_SAMPLE_GROUP_LINF:
		case GF_ISOM_SAMPLE_GROUP_TRIF:
		case GF_ISOM_SAMPLE_GROUP_NALM:
		case GF_ISOM_SAMPLE_GROUP_SAP:
		case GF_ISOM_SAMPLE_GROUP_SPOR:
		case GF_ISOM_SAMPLE_GROUP_SULM:
			continue;
		default:
			break;
		}
		if (*sgrp_idx>i) continue;

		for (j=0; j<gf_list_count(trak->Media->information->sampleTable->sampleGroups); j++) {
			sg = (GF_SampleGroupBox*)gf_list_get(trak->Media->information->sampleTable->sampleGroups, j);
			if (sg->grouping_type == sgd->grouping_type) break;
			sg = NULL;
		}
		u32 sgd_index = sgd->default_description_index;
		if (sg) {
			u32 snum=0;
			for (j=0; j<sg->entry_count; j++) {
				if (snum + sg->sample_entries[j].sample_count>= sample_number) {
					sgd_index = sg->sample_entries[j].group_description_index;
					break;
				}
				snum += sg->sample_entries[j].sample_count;
			}
		}

		*sgrp_type = sgd->grouping_type;
		if (sgrp_parameter && sg) *sgrp_parameter = sg->grouping_type_parameter;

		if (sgd_index) {
			GF_DefaultSampleGroupDescriptionEntry *entry = gf_list_get(sgd->group_descriptions, sgd_index-1);
			if (entry) {
				if (sgrp_data) *sgrp_data = entry->data;
				if (sgrp_size) *sgrp_size = entry->length;
			}
		}

		(*sgrp_idx)++;
		return GF_OK;
	}
	return GF_OK;
}