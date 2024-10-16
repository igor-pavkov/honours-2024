GF_Err snro_box_read(GF_Box *s, GF_BitStream *bs)
{
	GF_SeqOffHintEntryBox *ptr = (GF_SeqOffHintEntryBox *)s;
	ISOM_DECREASE_SIZE(ptr, 4);
	ptr->SeqOffset = gf_bs_read_u32(bs);
	return GF_OK;
}