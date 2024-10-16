GF_Err sdp_box_size(GF_Box *s)
{
	GF_SDPBox *ptr = (GF_SDPBox *)s;
	//don't count the NULL char!!!
	if (ptr->sdpText)
		ptr->size += strlen(ptr->sdpText);
	return GF_OK;
}