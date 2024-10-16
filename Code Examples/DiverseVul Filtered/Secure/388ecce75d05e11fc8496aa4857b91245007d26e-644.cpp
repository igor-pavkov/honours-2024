GF_Err npck_box_read(GF_Box *s, GF_BitStream *bs)
{
	GF_NPCKBox *ptr = (GF_NPCKBox *)s;
	ISOM_DECREASE_SIZE(ptr, 4);
	ptr->nbPackets = gf_bs_read_u32(bs);
	return GF_OK;
}