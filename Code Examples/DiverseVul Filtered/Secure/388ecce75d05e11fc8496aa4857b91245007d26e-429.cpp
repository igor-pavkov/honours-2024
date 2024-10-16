GF_Err name_box_write(GF_Box *s, GF_BitStream *bs)
{
	GF_Err e;
	GF_NameBox *ptr = (GF_NameBox *)s;
	if (ptr == NULL) return GF_BAD_PARAM;
	e = gf_isom_box_write_header(s, bs);
	if (e) return e;
	if (ptr->string) {
		gf_bs_write_data(bs, ptr->string, (u32) strlen(ptr->string) + 1);
	}
	return GF_OK;
}