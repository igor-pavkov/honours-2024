GF_Err rtp_hnti_box_size(GF_Box *s)
{
	GF_RTPBox *ptr = (GF_RTPBox *)s;
	ptr->size += 4 + strlen(ptr->sdpText);
	return GF_OK;
}