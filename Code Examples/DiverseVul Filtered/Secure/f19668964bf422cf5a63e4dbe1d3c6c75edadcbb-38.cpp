GF_Err ahdr_box_size(GF_Box *s)
{
	u32 pos=0;
	GF_AdobeDRMHeaderBox *ptr = (GF_AdobeDRMHeaderBox *)s;
	gf_isom_check_position(s, (GF_Box *)ptr->std_enc_params, &pos);
    return GF_OK;
}