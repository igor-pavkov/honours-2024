GF_Err smhd_box_write(GF_Box *s, GF_BitStream *bs)
{
	GF_Err e;
	GF_SoundMediaHeaderBox *ptr = (GF_SoundMediaHeaderBox *)s;
	e = gf_isom_full_box_write(s, bs);
	if (e) return e;
	gf_bs_write_u16(bs, ptr->balance);
	gf_bs_write_u16(bs, ptr->reserved);
	return GF_OK;
}