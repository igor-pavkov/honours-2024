GF_Err trpy_box_write(GF_Box *s, GF_BitStream *bs)
{
	GF_Err e;
	GF_TRPYBox *ptr = (GF_TRPYBox *)s;
	if (ptr == NULL) return GF_BAD_PARAM;

	e = gf_isom_box_write_header(s, bs);
	if (e) return e;
	gf_bs_write_u64(bs, ptr->nbBytes);
	return GF_OK;
}