GF_Err sdp_box_read(GF_Box *s, GF_BitStream *bs)
{
	u32 length;
	GF_SDPBox *ptr = (GF_SDPBox *)s;
	if (ptr == NULL) return GF_BAD_PARAM;

	length = (u32) (ptr->size);
	//sdp text has no delimiter !!!
	ptr->sdpText = (char*)gf_malloc(sizeof(char) * (length+1));
	if (!ptr->sdpText) return GF_OUT_OF_MEM;

	gf_bs_read_data(bs, ptr->sdpText, length);
	ptr->sdpText[length] = 0;
	return GF_OK;
}