GF_Err payt_box_write(GF_Box *s, GF_BitStream *bs)
{
	u32 len;
	GF_Err e;
	GF_PAYTBox *ptr = (GF_PAYTBox *)s;
	if (ptr == NULL) return GF_BAD_PARAM;
	e = gf_isom_box_write_header(s, bs);
	if (e) return e;
	gf_bs_write_u32(bs, ptr->payloadCode);
    len = ptr->payloadString ? (u32) strlen(ptr->payloadString) : 0;
	gf_bs_write_u8(bs, len);
	if (len) gf_bs_write_data(bs, ptr->payloadString, len);
	return GF_OK;
}