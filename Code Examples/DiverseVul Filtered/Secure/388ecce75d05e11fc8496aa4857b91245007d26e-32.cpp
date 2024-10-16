GF_Err rely_box_write(GF_Box *s, GF_BitStream *bs)
{
	GF_Err e;
	GF_RelyHintBox *ptr = (GF_RelyHintBox *)s;
	if (ptr == NULL) return GF_BAD_PARAM;
	e = gf_isom_box_write_header(s, bs);
	if (e) return e;
	gf_bs_write_int(bs, ptr->reserved, 6);
	gf_bs_write_int(bs, ptr->preferred, 1);
	gf_bs_write_int(bs, ptr->required, 1);
	return GF_OK;
}