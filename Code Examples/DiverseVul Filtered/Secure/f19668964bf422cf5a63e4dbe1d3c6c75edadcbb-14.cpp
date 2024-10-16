GF_Err iSFM_box_read(GF_Box *s, GF_BitStream *bs)
{
	GF_ISMASampleFormatBox *ptr = (GF_ISMASampleFormatBox *)s;

	ISOM_DECREASE_SIZE(ptr, 3);
	ptr->selective_encryption = gf_bs_read_int(bs, 1);
	gf_bs_read_int(bs, 7);
	ptr->key_indicator_length = gf_bs_read_u8(bs);
	ptr->IV_length = gf_bs_read_u8(bs);
	return GF_OK;
}