GF_Err totl_box_read(GF_Box *s, GF_BitStream *bs)
{
	GF_TOTLBox *ptr = (GF_TOTLBox *)s;
	ISOM_DECREASE_SIZE(ptr, 4);
	ptr->nbBytes = gf_bs_read_u32(bs);
	return GF_OK;
}