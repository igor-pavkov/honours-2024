GF_Err adkm_box_size(GF_Box *s)
{
	u32 pos=0;
	GF_AdobeDRMKeyManagementSystemBox *ptr = (GF_AdobeDRMKeyManagementSystemBox *)s;
	gf_isom_check_position(s, (GF_Box *)ptr->header, &pos);
	gf_isom_check_position(s, (GF_Box *)ptr->au_format, &pos);
    return GF_OK;
}