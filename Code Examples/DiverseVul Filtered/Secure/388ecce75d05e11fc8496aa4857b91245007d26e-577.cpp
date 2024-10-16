GF_Err dmax_box_write(GF_Box *s, GF_BitStream *bs)
{
	GF_Err e;
	GF_DMAXBox *ptr = (GF_DMAXBox *)s;
	if (ptr == NULL) return GF_BAD_PARAM;
	e = gf_isom_box_write_header(s, bs);
	if (e) return e;
	gf_bs_write_u32(bs, ptr->maxDur);
	return GF_OK;
}