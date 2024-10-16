GF_Err vmhd_box_write(GF_Box *s, GF_BitStream *bs)
{
	GF_Err e;
	GF_VideoMediaHeaderBox *ptr = (GF_VideoMediaHeaderBox *)s;

	e = gf_isom_full_box_write(s, bs);
	if (e) return e;
	gf_bs_write_u64(bs, ptr->reserved);
	return GF_OK;
}