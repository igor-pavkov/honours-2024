GF_Err smhd_box_read(GF_Box *s, GF_BitStream *bs)
{
	GF_SoundMediaHeaderBox *ptr = (GF_SoundMediaHeaderBox *)s;
	ISOM_DECREASE_SIZE(ptr, 4);
	ptr->balance = gf_bs_read_u16(bs);
	ptr->reserved = gf_bs_read_u16(bs);
	return GF_OK;
}