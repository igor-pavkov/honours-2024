GF_Err gf_isom_get_track_matrix(GF_ISOFile *the_file, u32 trackNumber, u32 matrix[9])
{
	GF_TrackBox *trak = gf_isom_get_track_from_file(the_file, trackNumber);
	if (!trak || !trak->Header) return GF_BAD_PARAM;
	memcpy(matrix, trak->Header->matrix, sizeof(trak->Header->matrix));
	return GF_OK;
}