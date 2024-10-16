GF_Err rtp_hnti_box_read(GF_Box *s, GF_BitStream *bs)
{
	u32 length;
	GF_RTPBox *ptr = (GF_RTPBox *)s;
	if (ptr == NULL) return GF_BAD_PARAM;

	ISOM_DECREASE_SIZE(ptr, 4)
	ptr->subType = gf_bs_read_u32(bs);

	length = (u32) (ptr->size);
	//sdp text has no delimiter !!!
	ptr->sdpText = (char*)gf_malloc(sizeof(char) * (length+1));
	if (!ptr->sdpText) return GF_OUT_OF_MEM;

	gf_bs_read_data(bs, ptr->sdpText, length);
	ptr->sdpText[length] = 0;
	return GF_OK;
}