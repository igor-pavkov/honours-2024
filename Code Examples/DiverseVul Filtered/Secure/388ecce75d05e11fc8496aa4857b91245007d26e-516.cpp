GF_Err hinf_box_write(GF_Box *s, GF_BitStream *bs)
{
//	GF_HintInfoBox *ptr = (GF_HintInfoBox *)s;
	if (!s) return GF_BAD_PARAM;
	return gf_isom_box_write_header(s, bs);
}