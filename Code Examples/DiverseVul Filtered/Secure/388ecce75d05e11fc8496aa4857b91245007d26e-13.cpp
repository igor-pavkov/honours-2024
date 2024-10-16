GF_Err dref_box_read(GF_Box *s, GF_BitStream *bs)
{
	GF_DataReferenceBox *ptr = (GF_DataReferenceBox *)s;
	ISOM_DECREASE_SIZE(ptr, 4);
	gf_bs_read_u32(bs);
	return gf_isom_box_array_read(s, bs, NULL);
}