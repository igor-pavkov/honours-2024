GF_Err mfhd_box_read(GF_Box *s, GF_BitStream *bs)
{
	GF_MovieFragmentHeaderBox *ptr = (GF_MovieFragmentHeaderBox *)s;
	ISOM_DECREASE_SIZE(ptr, 4);
	ptr->sequence_number = gf_bs_read_u32(bs);
	return GF_OK;
}