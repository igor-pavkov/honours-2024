GF_Box *akey_box_new()
{
	ISOM_DECL_BOX_ALLOC(GF_AdobeKeyInfoBox, GF_ISOM_BOX_TYPE_AKEY);
	tmp->version = 1;
	tmp->flags = 0;
	return (GF_Box *)tmp;
}