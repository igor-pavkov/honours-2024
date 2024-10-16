Bool gf_isom_is_smooth_streaming_moov(GF_ISOFile *the_file)
{
	return the_file ? the_file->is_smooth : GF_FALSE;
}