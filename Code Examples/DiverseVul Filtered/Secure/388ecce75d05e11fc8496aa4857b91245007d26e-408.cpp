GF_Err nump_box_read(GF_Box *s, GF_BitStream *bs)
{
	GF_NUMPBox *ptr = (GF_NUMPBox *)s;
	ISOM_DECREASE_SIZE(ptr, 8);
	ptr->nbPackets = gf_bs_read_u64(bs);
	return GF_OK;
}