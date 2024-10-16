Bool gf_isom_get_oinf_info(GF_ISOFile *file, u32 trackNumber, GF_OperatingPointsInformation **ptr)
{
	u32 oref_track, def_index=0;
	GF_TrackBox *trak = gf_isom_get_track_from_file(file, trackNumber);

	if (!ptr) return GF_FALSE;

	oref_track=0;
	gf_isom_get_reference(file, trackNumber, GF_ISOM_REF_OREF, 1, &oref_track);
	if (oref_track) {
		trak = gf_isom_get_track_from_file(file, oref_track);
		if (!trak) return GF_FALSE;
	}

	*ptr = (GF_OperatingPointsInformation *) gf_isom_get_sample_group_info_entry(file, trak, GF_ISOM_SAMPLE_GROUP_OINF, 1, &def_index, NULL);

	return *ptr ? GF_TRUE : GF_FALSE;
}