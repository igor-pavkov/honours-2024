const void *gf_isom_get_tfrf(GF_ISOFile *movie, u32 trackNumber)
{
#ifdef GPAC_DISABLE_ISOM_FRAGMENTS
	return NULL;
#else
	GF_TrackBox *trak = gf_isom_get_track_from_file(movie, trackNumber);
	if (!trak) return NULL;

	return trak->tfrf;
#endif
}