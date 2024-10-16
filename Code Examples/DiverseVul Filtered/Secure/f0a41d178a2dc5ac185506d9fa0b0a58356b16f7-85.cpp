u64 gf_isom_get_first_mdat_start(GF_ISOFile *movie)
{
	u64 offset;
	if (!movie) return 0;
	offset = movie->first_data_toplevel_offset + 8;
	if (movie->first_data_toplevel_size > 0xFFFFFFFFUL)
		offset += 8;
	return offset;
}