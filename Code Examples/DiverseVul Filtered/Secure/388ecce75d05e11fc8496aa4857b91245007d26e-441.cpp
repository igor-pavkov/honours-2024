GF_Err mfro_box_write(GF_Box *s, GF_BitStream *bs)
{
	GF_Err e;
	GF_MovieFragmentRandomAccessOffsetBox *ptr = (GF_MovieFragmentRandomAccessOffsetBox *)s;

	e = gf_isom_full_box_write(s, bs);
	if (e) return e;

	gf_bs_write_u32(bs, ptr->container_size);
	return GF_OK;
}