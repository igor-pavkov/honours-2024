u64 gf_isom_get_track_magic(GF_ISOFile *movie, u32 trackNumber)
{
	GF_TrackBox *trak = gf_isom_get_track_from_file(movie, trackNumber);
	if (!trak) return 0;
	return trak->magic;
}