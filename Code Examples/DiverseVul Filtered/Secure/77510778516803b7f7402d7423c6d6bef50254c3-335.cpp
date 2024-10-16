GF_Err rtp_hnti_box_read(GF_Box *s, GF_BitStream *bs)
{
	u32 length;
	GF_RTPBox *ptr = (GF_RTPBox *)s;
	if (ptr == NULL) return GF_BAD_PARAM;

	ISOM_DECREASE_SIZE(ptr, 4)
	ptr->subType = gf_bs_read_u32(bs);

	length = (u32) (ptr->size);

	if (length >= (u32)0xFFFFFFFF) {
		GF_LOG(GF_LOG_ERROR, GF_LOG_CONTAINER, ("[iso file] Invalid length %lu in rtp_hnti box\n", length));
		return GF_ISOM_INVALID_FILE;
	}

	//sdp text has no delimiter !!!
	ptr->sdpText = (char*)gf_malloc(sizeof(char) * (length+1));
	if (!ptr->sdpText) return GF_OUT_OF_MEM;

	gf_bs_read_data(bs, ptr->sdpText, length);
	ptr->sdpText[length] = 0;
	return GF_OK;
}