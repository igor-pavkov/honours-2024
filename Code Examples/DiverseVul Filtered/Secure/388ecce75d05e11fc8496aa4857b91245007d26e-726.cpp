GF_Err rtpo_box_read(GF_Box *s, GF_BitStream *bs)
{
	GF_RTPOBox *ptr = (GF_RTPOBox *)s;
	ISOM_DECREASE_SIZE(ptr, 4);
	ptr->timeOffset = gf_bs_read_u32(bs);
	return GF_OK;
}