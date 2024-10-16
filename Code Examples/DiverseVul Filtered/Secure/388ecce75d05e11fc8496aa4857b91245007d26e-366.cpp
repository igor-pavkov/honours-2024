void rtp_hnti_box_del(GF_Box *s)
{
	GF_RTPBox *ptr = (GF_RTPBox *)s;
	if (ptr->sdpText) gf_free(ptr->sdpText);
	gf_free(ptr);

}