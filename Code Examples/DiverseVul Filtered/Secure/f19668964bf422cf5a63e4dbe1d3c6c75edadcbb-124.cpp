GF_Err aprm_box_size(GF_Box *s)
{
	u32 pos=0;
	GF_AdobeStdEncryptionParamsBox *ptr = (GF_AdobeStdEncryptionParamsBox *)s;
	gf_isom_check_position(s, (GF_Box *)ptr->enc_info, &pos);
	gf_isom_check_position(s, (GF_Box *)ptr->key_info, &pos);
    return GF_OK;
}