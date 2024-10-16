GF_Err tmin_box_read(GF_Box *s, GF_BitStream *bs)
{
	GF_TMINBox *ptr = (GF_TMINBox *)s;
	ISOM_DECREASE_SIZE(ptr, 4)
	ptr->minTime = gf_bs_read_u32(bs);
	return GF_OK;
}