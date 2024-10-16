GF_Err fiel_box_write(GF_Box *s, GF_BitStream *bs)
{
	GF_Err e;
	GF_FieldInfoBox *ptr = (GF_FieldInfoBox *)s;
	e = gf_isom_box_write_header(s, bs);
	if (e) return e;
	gf_bs_write_u8(bs, ptr->field_count);
	gf_bs_write_u8(bs, ptr->field_order);
	return GF_OK;
}