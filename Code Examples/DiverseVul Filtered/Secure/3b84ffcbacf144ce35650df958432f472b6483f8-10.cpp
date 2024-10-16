Bool gf_isom_cenc_has_saiz_saio_track(GF_SampleTableBox *stbl, u32 scheme_type)
{
	return gf_isom_cenc_has_saiz_saio_full(stbl, NULL, scheme_type);
}