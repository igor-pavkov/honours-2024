void sdp_box_del(GF_Box *s)
{
	GF_SDPBox *ptr = (GF_SDPBox *)s;
	if (ptr->sdpText) gf_free(ptr->sdpText);
	gf_free(ptr);

}