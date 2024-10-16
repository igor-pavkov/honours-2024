GF_Err unkn_box_write(GF_Box *s, GF_BitStream *bs)
{
	GF_Err e;
	u32 type;
	GF_UnknownBox *ptr = (GF_UnknownBox *)s;
	if (!s) return GF_BAD_PARAM;
	type = s->type;
	ptr->type = ptr->original_4cc;
	e = gf_isom_box_write_header(s, bs);
	ptr->type = type;
	if (e) return e;

	if (ptr->dataSize && ptr->data) {
		gf_bs_write_data(bs, ptr->data, ptr->dataSize);
	}
	return GF_OK;
}