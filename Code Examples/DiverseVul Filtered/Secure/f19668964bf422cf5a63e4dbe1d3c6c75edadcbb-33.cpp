GF_Box *aeib_box_new()
{
	ISOM_DECL_BOX_ALLOC(GF_AdobeEncryptionInfoBox, GF_ISOM_BOX_TYPE_AEIB);
	tmp->version = 1;
	tmp->flags = 0;
	return (GF_Box *)tmp;
}