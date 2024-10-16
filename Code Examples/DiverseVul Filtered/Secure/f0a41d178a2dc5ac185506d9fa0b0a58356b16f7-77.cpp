u64 gf_isom_get_unused_box_bytes(GF_ISOFile *movie)
{
	u64 size = unused_bytes;
	u32 i, count;
	if (!movie) return 0;
	count = gf_list_count(movie->TopBoxes);
	for (i=0; i<count; i++) {
		GF_Box *b = gf_list_get(movie->TopBoxes, i);
		size += box_unused_bytes(b);
	}
	return size;
}