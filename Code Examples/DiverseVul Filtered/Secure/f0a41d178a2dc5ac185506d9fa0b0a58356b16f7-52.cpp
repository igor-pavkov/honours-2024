u32 gf_isom_get_track_group(GF_ISOFile *file, u32 track_number, u32 track_group_type)
{
	u32 i;
	GF_TrackGroupTypeBox *trgt;
	GF_TrackBox *trak;

	trak = gf_isom_get_track_from_file(file, track_number);
	if (!trak) return 0;
	if (!trak->groups) return 0;

	for (i=0; i<gf_list_count(trak->groups->groups); i++) {
		trgt = gf_list_get(trak->groups->groups, i);
		if (trgt->group_type == track_group_type) {
			return trgt->track_group_id;
		}
	}
	return 0;
}