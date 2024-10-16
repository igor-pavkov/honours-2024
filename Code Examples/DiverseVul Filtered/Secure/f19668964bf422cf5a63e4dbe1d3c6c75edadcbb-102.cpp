GF_Box *adkm_box_new()
{
	ISOM_DECL_BOX_ALLOC(GF_AdobeDRMKeyManagementSystemBox, GF_ISOM_BOX_TYPE_ADKM);
	tmp->version = 1;
	tmp->flags = 0;
	return (GF_Box *)tmp;
}