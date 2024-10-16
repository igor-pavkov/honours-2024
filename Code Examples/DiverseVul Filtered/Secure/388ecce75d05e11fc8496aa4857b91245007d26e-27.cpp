GF_Err maxr_box_read(GF_Box *s, GF_BitStream *bs)
{
	GF_MAXRBox *ptr = (GF_MAXRBox *)s;
	if (ptr == NULL) return GF_BAD_PARAM;
	ISOM_DECREASE_SIZE(ptr, 8);
	ptr->granularity = gf_bs_read_u32(bs);
	ptr->maxDataRate = gf_bs_read_u32(bs);
	return GF_OK;
}