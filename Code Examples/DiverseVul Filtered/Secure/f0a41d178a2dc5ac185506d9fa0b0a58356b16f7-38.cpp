GF_Err gf_isom_text_set_streaming_mode(GF_ISOFile *movie, Bool do_convert)
{
	if (!movie) return GF_BAD_PARAM;
	movie->convert_streaming_text = do_convert;
	return GF_OK;
}