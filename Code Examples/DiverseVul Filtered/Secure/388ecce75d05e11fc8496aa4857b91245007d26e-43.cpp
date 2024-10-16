GF_Err dmax_box_read(GF_Box *s, GF_BitStream *bs)
{
	GF_DMAXBox *ptr = (GF_DMAXBox *)s;
	ISOM_DECREASE_SIZE(ptr, 4)
	ptr->maxDur = gf_bs_read_u32(bs);
	return GF_OK;
}