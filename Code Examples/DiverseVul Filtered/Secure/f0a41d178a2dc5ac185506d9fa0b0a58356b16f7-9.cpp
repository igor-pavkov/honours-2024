Bool gf_isom_moov_first(GF_ISOFile *movie)
{
	u32 i;
	for (i=0; i<gf_list_count(movie->TopBoxes); i++) {
		GF_Box *b = (GF_Box*)gf_list_get(movie->TopBoxes, i);
		if (b->type == GF_ISOM_BOX_TYPE_MOOV) return GF_TRUE;
		if (b->type == GF_ISOM_BOX_TYPE_MDAT) return GF_FALSE;
	}
	return GF_FALSE;
}