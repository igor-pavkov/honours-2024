GF_Err fiin_box_read(GF_Box *s, GF_BitStream *bs)
{
	FDItemInformationBox *ptr = (FDItemInformationBox *)s;

	ISOM_DECREASE_SIZE(ptr, 2);
	gf_bs_read_u16(bs);
	return gf_isom_box_array_read(s, bs);
}