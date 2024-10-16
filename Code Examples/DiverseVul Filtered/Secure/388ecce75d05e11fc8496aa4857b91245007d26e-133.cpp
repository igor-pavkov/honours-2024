GF_Err tims_box_read(GF_Box *s, GF_BitStream *bs)
{
	GF_TSHintEntryBox *ptr = (GF_TSHintEntryBox *)s;
	ISOM_DECREASE_SIZE(ptr, 4);
	ptr->timeScale = gf_bs_read_u32(bs);
	return GF_OK;
}