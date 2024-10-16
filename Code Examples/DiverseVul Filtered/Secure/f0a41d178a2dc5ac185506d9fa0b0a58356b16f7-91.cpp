Bool gf_isom_has_keep_utc_times(GF_ISOFile *file)
{
	if (!file) return GF_FALSE;
	return file->keep_utc;
}