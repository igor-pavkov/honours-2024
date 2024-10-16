GF_Box *vvcc_box_new()
{
	GF_VVCConfigurationBox *tmp = (GF_VVCConfigurationBox *) gf_malloc(sizeof(GF_VVCConfigurationBox));
	if (tmp == NULL) return NULL;
	memset(tmp, 0, sizeof(GF_VVCConfigurationBox));
	tmp->type = GF_ISOM_BOX_TYPE_HVCC;
	return (GF_Box *)tmp;
}