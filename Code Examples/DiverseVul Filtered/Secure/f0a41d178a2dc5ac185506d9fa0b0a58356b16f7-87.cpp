void gf_isom_keep_utc_times(GF_ISOFile *file, Bool keep_utc)
{
	if (!file) return;
	file->keep_utc = keep_utc;
}