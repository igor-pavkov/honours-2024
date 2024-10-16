GF_Err tmax_box_read(GF_Box *s, GF_BitStream *bs)
{
	GF_TMAXBox *ptr = (GF_TMAXBox *)s;
	ISOM_DECREASE_SIZE(ptr, 4)
	ptr->maxTime = gf_bs_read_u32(bs);
	return GF_OK;
}