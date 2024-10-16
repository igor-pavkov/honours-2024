GF_Err mfro_box_read(GF_Box *s, GF_BitStream *bs)
{
	GF_MovieFragmentRandomAccessOffsetBox *ptr = (GF_MovieFragmentRandomAccessOffsetBox *)s;

	ISOM_DECREASE_SIZE(ptr, 4);
	ptr->container_size = gf_bs_read_u32(bs);
	return GF_OK;
}