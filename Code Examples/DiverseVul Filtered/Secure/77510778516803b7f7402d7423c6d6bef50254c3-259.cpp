GF_Err tsro_box_read(GF_Box *s, GF_BitStream *bs)
{
	GF_TimeOffHintEntryBox *ptr = (GF_TimeOffHintEntryBox *)s;
	ISOM_DECREASE_SIZE(ptr, 4);
	ptr->TimeOffset = gf_bs_read_u32(bs);
	return GF_OK;
}