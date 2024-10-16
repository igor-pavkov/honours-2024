GF_Err clef_box_write(GF_Box *s, GF_BitStream *bs)
{
	GF_Err e;
	GF_ApertureBox *ptr = (GF_ApertureBox *)s;

	e = gf_isom_full_box_write(s, bs);
	if (e) return e;
	gf_bs_write_u32(bs, ptr->width);
	gf_bs_write_u32(bs, ptr->height);
	return GF_OK;
}