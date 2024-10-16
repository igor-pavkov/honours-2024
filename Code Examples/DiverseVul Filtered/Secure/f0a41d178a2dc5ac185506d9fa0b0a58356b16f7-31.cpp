u64 gf_isom_get_current_tfdt(GF_ISOFile *the_file, u32 trackNumber)
{
#ifdef	GPAC_DISABLE_ISOM_FRAGMENTS
	return 0;
#else
	GF_TrackBox *trak;
	trak = gf_isom_get_track_from_file(the_file, trackNumber);
	if (!trak) return 0;
	return trak->dts_at_seg_start;
#endif
}