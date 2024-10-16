GF_Box *aprm_box_new()
{
	ISOM_DECL_BOX_ALLOC(GF_AdobeStdEncryptionParamsBox, GF_ISOM_BOX_TYPE_APRM);
	tmp->version = 1;
	tmp->flags = 0;
	return (GF_Box *)tmp;
}