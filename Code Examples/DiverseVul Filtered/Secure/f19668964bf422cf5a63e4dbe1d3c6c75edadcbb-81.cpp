GF_Err odkm_box_size(GF_Box *s)
{
	u32 pos=0;
	GF_OMADRMKMSBox *ptr = (GF_OMADRMKMSBox *)s;
	gf_isom_check_position(s, (GF_Box *)ptr->hdr, &pos);
	gf_isom_check_position(s, (GF_Box *)ptr->fmt, &pos);
	return GF_OK;
}