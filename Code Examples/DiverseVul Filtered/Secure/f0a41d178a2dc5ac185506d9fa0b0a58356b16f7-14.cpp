GF_ISONaluExtractMode gf_isom_get_nalu_extract_mode(GF_ISOFile *the_file, u32 trackNumber)
{
	GF_TrackBox *trak = gf_isom_get_track_from_file(the_file, trackNumber);
	if (!trak) return 0;
	return trak->extractor_mode;
}