void gf_isom_disable_odf_conversion(GF_ISOFile *movie, Bool disable)
{
	if (movie) movie->disable_odf_translate = disable ? 1 : 0;
}