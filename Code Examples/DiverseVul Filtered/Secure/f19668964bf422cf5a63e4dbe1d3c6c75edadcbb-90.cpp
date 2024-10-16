GF_Err adaf_box_write(GF_Box *s, GF_BitStream *bs)
{
	GF_Err e;
	GF_AdobeDRMAUFormatBox *ptr = (GF_AdobeDRMAUFormatBox *) s;
	if (!s) return GF_BAD_PARAM;
	e = gf_isom_full_box_write(s, bs);
	if (e) return e;

	gf_bs_write_u8(bs, ptr->selective_enc);
	gf_bs_write_u8(bs, 0x0);
	gf_bs_write_u8(bs, ptr->IV_length);
	return GF_OK;
}