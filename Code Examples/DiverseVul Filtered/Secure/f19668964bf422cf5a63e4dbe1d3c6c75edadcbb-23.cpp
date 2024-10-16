GF_Err akey_box_size(GF_Box *s)
{
	u32 pos=0;
	GF_AdobeKeyInfoBox *ptr = (GF_AdobeKeyInfoBox *)s;
	gf_isom_check_position(s, (GF_Box *)ptr->params, &pos);
    return GF_OK;
}