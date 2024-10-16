u32 gf_isom_sample_has_subsamples(GF_ISOFile *movie, u32 track, u32 sampleNumber, u32 flags)
{
	GF_TrackBox *trak = gf_isom_get_track_from_file(movie, track);
	if (!trak) return GF_BAD_PARAM;
	if (!trak->Media->information->sampleTable->sub_samples) return 0;
	if (!sampleNumber) return 1;
	return gf_isom_sample_get_subsample_entry(movie, track, sampleNumber, flags, NULL);
}