u64 gf_isom_get_fragmented_duration(GF_ISOFile *movie)
{
	if (movie->moov->mvex && movie->moov->mvex->mehd)
		return movie->moov->mvex->mehd->fragment_duration;

	return 0;
}