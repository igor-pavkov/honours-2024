Bool gf_isom_cenc_has_saiz_saio_traf(GF_TrackFragmentBox *traf, u32 scheme_type)
{
	return gf_isom_cenc_has_saiz_saio_full(NULL, traf, scheme_type);
}