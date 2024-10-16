GF_Err gf_isom_set_nalu_extract_mode(GF_ISOFile *the_file, u32 trackNumber, GF_ISONaluExtractMode nalu_extract_mode)
{
	GF_TrackReferenceTypeBox *dpnd;
	GF_TrackBox *trak = gf_isom_get_track_from_file(the_file, trackNumber);
	if (!trak) return GF_BAD_PARAM;
	trak->extractor_mode = nalu_extract_mode;

	if (!trak->References) return GF_OK;

	/*get base*/
	dpnd = NULL;
	trak->has_base_layer = GF_FALSE;
	Track_FindRef(trak, GF_ISOM_REF_SCAL, &dpnd);
	if (dpnd) trak->has_base_layer = GF_TRUE;
	return GF_OK;
}