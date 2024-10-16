GF_Err adaf_box_read(GF_Box *s, GF_BitStream *bs)
{
	GF_AdobeDRMAUFormatBox *ptr = (GF_AdobeDRMAUFormatBox*)s;

	ISOM_DECREASE_SIZE(ptr, 3);
	ptr->selective_enc = gf_bs_read_u8(bs);
	gf_bs_read_u8(bs);//resersed
	ptr->IV_length = gf_bs_read_u8(bs);
	return GF_OK;
}