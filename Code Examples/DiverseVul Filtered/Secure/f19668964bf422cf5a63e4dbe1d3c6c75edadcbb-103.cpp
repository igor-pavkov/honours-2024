GF_Err sinf_box_size(GF_Box *s)
{
	u32 pos=0;
	GF_ProtectionSchemeInfoBox *ptr = (GF_ProtectionSchemeInfoBox *)s;
	gf_isom_check_position(s, (GF_Box *)ptr->original_format, &pos);
	gf_isom_check_position(s, (GF_Box *)ptr->scheme_type, &pos);
	gf_isom_check_position(s, (GF_Box *)ptr->info, &pos);
    return GF_OK;
}