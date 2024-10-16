GF_Err sdp_box_read(GF_Box *s, GF_BitStream *bs)
{
	u32 length;
	GF_SDPBox *ptr = (GF_SDPBox *)s;
	if (ptr == NULL) return GF_BAD_PARAM;

	length = (u32) (ptr->size);

	if (length >= (u32)0xFFFFFFFF) {
		GF_LOG(GF_LOG_ERROR, GF_LOG_CONTAINER, ("[iso file] Invalid length %lu in sdp box\n", length));
		return GF_ISOM_INVALID_FILE;
	}

	//sdp text has no delimiter !!!
	ptr->sdpText = (char*)gf_malloc(sizeof(char) * (length+1));
	if (!ptr->sdpText) return GF_OUT_OF_MEM;

	gf_bs_read_data(bs, ptr->sdpText, length);
	ptr->sdpText[length] = 0;
	return GF_OK;
}