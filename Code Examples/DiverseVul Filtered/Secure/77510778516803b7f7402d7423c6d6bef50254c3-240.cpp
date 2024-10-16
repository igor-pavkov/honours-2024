GF_Err extr_box_write(GF_Box *s, GF_BitStream *bs)
{
	GF_Err e;
	GF_ExtraDataBox *ptr = (GF_ExtraDataBox *) s;
	if (!s) return GF_BAD_PARAM;

	e = gf_isom_box_write_header(s, bs);
	if (e) return e;

	if (ptr->feci) {
		e = gf_isom_box_write((GF_Box *)ptr->feci, bs);
		if (e) return e;
	}
	gf_bs_write_data(bs, ptr->data, ptr->data_length);
	return GF_OK;
}