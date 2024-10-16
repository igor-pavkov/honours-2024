GF_Err frma_box_write(GF_Box *s, GF_BitStream *bs)
{
	GF_Err e;
	GF_OriginalFormatBox *ptr = (GF_OriginalFormatBox *)s;
	if (!s) return GF_BAD_PARAM;
	e = gf_isom_box_write_header(s, bs);
	if (e) return e;
	gf_bs_write_u32(bs, ptr->data_format);
	return GF_OK;
}