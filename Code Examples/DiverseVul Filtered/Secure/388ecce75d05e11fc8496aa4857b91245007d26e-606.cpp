GF_Err dimm_box_read(GF_Box *s, GF_BitStream *bs)
{
	GF_DIMMBox *ptr = (GF_DIMMBox *)s;
	ISOM_DECREASE_SIZE(ptr, 8)
	ptr->nbBytes = gf_bs_read_u64(bs);
	return GF_OK;
}