GF_Err gf_isom_get_sample_rap_roll_info(GF_ISOFile *the_file, u32 trackNumber, u32 sample_number, Bool *is_rap, GF_ISOSampleRollType *roll_type, s32 *roll_distance)
{
	GF_TrackBox *trak;
	u32 i, count;

	if (is_rap) *is_rap = GF_FALSE;
	if (roll_type) *roll_type = 0;
	if (roll_distance) *roll_distance = 0;

	trak = gf_isom_get_track_from_file(the_file, trackNumber);
	if (!trak) return GF_BAD_PARAM;
	if (!trak->Media->information->sampleTable->sampleGroups) return GF_OK;

	if (!sample_number) {
		count = gf_list_count(trak->Media->information->sampleTable->sampleGroupsDescription);
		for (i=0; i<count; i++) {
			GF_SampleGroupDescriptionBox *sgdesc = (GF_SampleGroupDescriptionBox*)gf_list_get(trak->Media->information->sampleTable->sampleGroupsDescription, i);
			switch (sgdesc->grouping_type) {
			case GF_ISOM_SAMPLE_GROUP_RAP:
			case GF_ISOM_SAMPLE_GROUP_SYNC:
				if (is_rap) *is_rap = GF_TRUE;
				break;
			case GF_ISOM_SAMPLE_GROUP_ROLL:
			case GF_ISOM_SAMPLE_GROUP_PROL:
				if (roll_type)
					*roll_type = (sgdesc->grouping_type==GF_ISOM_SAMPLE_GROUP_PROL) ? GF_ISOM_SAMPLE_PREROLL : GF_ISOM_SAMPLE_ROLL;
				if (roll_distance) {
					s32 max_roll = 0;
					u32 j;
					for (j=0; j<gf_list_count(sgdesc->group_descriptions); j++) {
						GF_RollRecoveryEntry *roll_entry = (GF_RollRecoveryEntry*)gf_list_get(sgdesc->group_descriptions, j);
						if (max_roll < roll_entry->roll_distance)
							max_roll = roll_entry->roll_distance;
					}
					if (*roll_distance < max_roll) *roll_distance = max_roll;
				}
				break;
			}
		}
		return GF_OK;
	}

	count = gf_list_count(trak->Media->information->sampleTable->sampleGroups);
	for (i=0; i<count; i++) {
		GF_SampleGroupBox *sg;
		u32 j, group_desc_index;
		GF_SampleGroupDescriptionBox *sgdesc;
		u32 first_sample_in_entry, last_sample_in_entry;
		first_sample_in_entry = 1;
		group_desc_index = 0;
		sg = (GF_SampleGroupBox*)gf_list_get(trak->Media->information->sampleTable->sampleGroups, i);
		for (j=0; j<sg->entry_count; j++) {
			last_sample_in_entry = first_sample_in_entry + sg->sample_entries[j].sample_count - 1;
			if ((sample_number<first_sample_in_entry) || (sample_number>last_sample_in_entry)) {
				first_sample_in_entry = last_sample_in_entry+1;
				continue;
			}
			/*we found our sample*/
			group_desc_index = sg->sample_entries[j].group_description_index;
			break;
		}
		/*no sampleGroup info associated*/
		if (!group_desc_index) continue;

		sgdesc = NULL;
		for (j=0; j<gf_list_count(trak->Media->information->sampleTable->sampleGroupsDescription); j++) {
			sgdesc = (GF_SampleGroupDescriptionBox*)gf_list_get(trak->Media->information->sampleTable->sampleGroupsDescription, j);
			if (sgdesc->grouping_type==sg->grouping_type) break;
			sgdesc = NULL;
		}
		/*no sampleGroup description found for this group (invalid file)*/
		if (!sgdesc) continue;

		switch (sgdesc->grouping_type) {
		case GF_ISOM_SAMPLE_GROUP_RAP:
		case GF_ISOM_SAMPLE_GROUP_SYNC:
			if (is_rap) *is_rap = GF_TRUE;
			break;
		case GF_ISOM_SAMPLE_GROUP_ROLL:
		case GF_ISOM_SAMPLE_GROUP_PROL:
			if (roll_type)
				*roll_type = (sgdesc->grouping_type==GF_ISOM_SAMPLE_GROUP_PROL) ? GF_ISOM_SAMPLE_PREROLL : GF_ISOM_SAMPLE_ROLL;

			if (roll_distance) {
				GF_RollRecoveryEntry *roll_entry = (GF_RollRecoveryEntry *) gf_list_get(sgdesc->group_descriptions, group_desc_index - 1);
				if (roll_entry)
					*roll_distance = roll_entry->roll_distance;
			}
			break;
		}
	}
	return GF_OK;
}