GF_Err pmax_box_read(GF_Box *s, GF_BitStream *bs)
{
	GF_PMAXBox *ptr = (GF_PMAXBox *)s;
	ISOM_DECREASE_SIZE(ptr, 4)
	ptr->maxSize = gf_bs_read_u32(bs);
	return GF_OK;
}