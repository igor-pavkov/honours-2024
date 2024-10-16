GF_Err iSFM_box_write(GF_Box *s, GF_BitStream *bs)
{
	GF_Err e;
	GF_ISMASampleFormatBox *ptr = (GF_ISMASampleFormatBox *)s;
	if (!s) return GF_BAD_PARAM;
	e = gf_isom_full_box_write(s, bs);
	if (e) return e;
	gf_bs_write_int(bs, ptr->selective_encryption, 1);
	gf_bs_write_int(bs, 0, 7);
	gf_bs_write_u8(bs, ptr->key_indicator_length);
	gf_bs_write_u8(bs, ptr->IV_length);
	return GF_OK;
}