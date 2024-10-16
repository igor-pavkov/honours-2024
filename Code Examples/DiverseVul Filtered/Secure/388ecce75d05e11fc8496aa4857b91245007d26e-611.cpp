GF_Err sdp_box_write(GF_Box *s, GF_BitStream *bs)
{
	GF_Err e;
	GF_SDPBox *ptr = (GF_SDPBox *)s;
	if (ptr == NULL) return GF_BAD_PARAM;
	e = gf_isom_box_write_header(s, bs);
	if (e) return e;
	//don't write the NULL char!!!
	if (ptr->sdpText)
		gf_bs_write_data(bs, ptr->sdpText, (u32) strlen(ptr->sdpText));
	return GF_OK;
}