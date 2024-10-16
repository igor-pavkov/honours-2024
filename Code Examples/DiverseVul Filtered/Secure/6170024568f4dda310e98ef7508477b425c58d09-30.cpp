GF_Err clef_box_read(GF_Box *s, GF_BitStream *bs)
{
	GF_ApertureBox *ptr = (GF_ApertureBox *)s;
	ISOM_DECREASE_SIZE(ptr, 8);
	ptr->width = gf_bs_read_u32(bs);
	ptr->height = gf_bs_read_u32(bs);
	return GF_OK;
}