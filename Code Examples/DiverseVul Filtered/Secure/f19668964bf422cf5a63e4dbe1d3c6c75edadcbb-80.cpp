GF_Err frma_box_read(GF_Box *s, GF_BitStream *bs)
{
	GF_OriginalFormatBox *ptr = (GF_OriginalFormatBox *)s;
	ISOM_DECREASE_SIZE(ptr, 4);
	ptr->data_format = gf_bs_read_u32(bs);
	return GF_OK;
}