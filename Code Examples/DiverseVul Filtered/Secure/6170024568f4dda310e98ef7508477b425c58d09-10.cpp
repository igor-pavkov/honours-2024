GF_Err ilst_box_write(GF_Box *s, GF_BitStream *bs)
{
	GF_Err e;
//	GF_ItemListBox *ptr = (GF_ItemListBox *)s;

	e = gf_isom_box_write_header(s, bs);
	if (e) return e;

	return GF_OK;
}