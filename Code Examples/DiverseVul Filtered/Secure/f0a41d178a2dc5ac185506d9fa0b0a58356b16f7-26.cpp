const char *gf_isom_get_filename(GF_ISOFile *movie)
{
	if (!movie) return NULL;
#ifndef GPAC_DISABLE_ISOM_WRITE
	if (movie->finalName && !movie->fileName) return movie->finalName;
#endif
	return movie->fileName;
}