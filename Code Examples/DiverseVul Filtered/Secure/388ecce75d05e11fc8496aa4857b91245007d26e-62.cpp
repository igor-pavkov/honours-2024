GF_Err rtp_hnti_box_write(GF_Box *s, GF_BitStream *bs)
{
	GF_Err e;
	GF_RTPBox *ptr = (GF_RTPBox *)s;
	if (ptr == NULL) return GF_BAD_PARAM;
	e = gf_isom_box_write_header(s, bs);
	if (e) return e;
	gf_bs_write_u32(bs, ptr->subType);
	//don't write the NULL char!!!
	gf_bs_write_data(bs, ptr->sdpText, (u32) strlen(ptr->sdpText));
	return GF_OK;
}