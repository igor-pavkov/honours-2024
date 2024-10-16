GF_Err uuid_box_write(GF_Box *s, GF_BitStream *bs)
{
	GF_Err e;
	GF_UnknownUUIDBox *ptr = (GF_UnknownUUIDBox*)s;
	if (!s) return GF_BAD_PARAM;

	e = gf_isom_box_write_header(s, bs);
	if (e) return e;
	if (ptr->data) {
		gf_bs_write_data(bs, ptr->data, ptr->dataSize);
	}
	return GF_OK;
}